#include "esp_at.h"

#include "string_helper.h"

#define ESP_LINE_BUFFER_SIZE 256U
#define ESP_PAYLOAD_BUFFER_SIZE 256U
#define ESP_TX_BUFFER_SIZE 2048U
#define ESP_WIFI_COMMAND_BUFFER_SIZE 128U
#define ESP_COMMAND_BUFFER_SIZE 32U
#define ESP_SSID_MAX_LENGTH 32U
#define ESP_PASSWORD_MAX_LENGTH 64U
#define ESP_IP_OCTET_COUNT 4U
#define ESP_CONNECTION_ID_INVALID 0xFFU

#define ESP_TIMEOUT_MS 3000UL
#define ESP_JOIN_TIMEOUT_MS 20000UL
#define ESP_IPD_TIMEOUT_MS 3000UL
#define ESP_TX_TIMEOUT_MS 3000UL

typedef enum {
	ESP_Command_None = 0,
	ESP_Command_AT,
	ESP_Command_CWMODE,
	ESP_Command_CWJAP,
	ESP_Command_CIFSR,
	ESP_Command_CIPMUX,
	ESP_Command_CIPSERVERMAXCONN,
	ESP_Command_CIPSERVER,
	ESP_Command_CIPSTATUS,
	ESP_Command_Test,
} ESP_Command;

typedef enum {
	ESP_RX_Mode_Line,
	ESP_RX_Mode_IPDPayload,
} ESP_RX_Mode;

typedef enum {
	ESP_TX_State_Idle,
	ESP_TX_State_WaitPrompt,
	ESP_TX_State_WaitSendResult,
} ESP_TX_State;

typedef struct {
	ESP_Config config;
	uint8_t initialized;

	ESP_RX_Mode rx_mode;
	ESP_TX_State tx_state;
	ESP_Mode mode;
	ESP_WifiState wifi_state;
	uint8_t ip[ESP_IP_OCTET_COUNT];

	ESP_TCPState tcp_state;
	ESP_ConnectionMode connection_mode;
	ESP_ServerState server_state;
	uint8_t active_connection_id;

	uint8_t metadata_parsed;
	uint8_t payload_discard;
	uint8_t rx_connection_id;
	uint32_t payload_position;
	uint32_t payload_buffer_length;
	uint32_t payload_length;
	uint32_t ipd_started_at;
	char payload[ESP_PAYLOAD_BUFFER_SIZE];
	ESP_TCPReceiveFn tcp_receive;
	void* tcp_receive_context;

	uint8_t tx_connection_id;
	uint32_t tx_length;
	uint32_t tx_started_at;
	uint8_t tx_prompt_space_pending;
	char tx_data[ESP_TX_BUFFER_SIZE];

	char line[ESP_LINE_BUFFER_SIZE];
	uint32_t line_length;
	uint8_t line_overflow;

	ESP_Command command;
	uint8_t command_waiting;
	ESP_Result command_result;
	uint8_t response_parsed;
	uint8_t parsed_ip[ESP_IP_OCTET_COUNT];
} ESP_Handle;

static ESP_Handle esp_handle;
static volatile uint32_t esp_millis;
static uint32_t connection_generation;

void ESP_Tick(void) {
	esp_millis++;
}

static void ESP_IP_Reset(void) {
	for (uint32_t i = 0; i < ESP_IP_OCTET_COUNT; i++) {
		esp_handle.ip[i] = 0;
	}
}

static void ESP_NetworkDetails_Reset(void) {
	ESP_IP_Reset();
}

static void ESP_TCPDetails_Reset(void) {
	connection_generation++;
	esp_handle.active_connection_id = ESP_CONNECTION_ID_INVALID;
	esp_handle.tcp_state = ESP_TCPState_Disconnected;
	esp_handle.connection_mode = ESP_ConnectionMode_Single;
	esp_handle.server_state = ESP_ServerState_NotStarted;
}

static void ESP_TX_Reset(void) {
	esp_handle.tx_state = ESP_TX_State_Idle;
	esp_handle.tx_connection_id = ESP_CONNECTION_ID_INVALID;
	esp_handle.tx_length = 0;
	esp_handle.tx_started_at = 0;
	esp_handle.tx_prompt_space_pending = 0;
	esp_handle.tx_data[0] = '\0';
}

static void ESP_TCPConnection_Reset(void) {
	connection_generation++;
	esp_handle.active_connection_id = ESP_CONNECTION_ID_INVALID;
	esp_handle.tcp_state = ESP_TCPState_Disconnected;
	ESP_TX_Reset();
}

static void ESP_Payload_Reset(void) {
	esp_handle.metadata_parsed = 0;
	esp_handle.payload_discard = 0;
	esp_handle.rx_connection_id = ESP_CONNECTION_ID_INVALID;
	esp_handle.payload_position= 0;
	esp_handle.payload_buffer_length = 0;
	esp_handle.payload_length = 0;
	esp_handle.ipd_started_at = 0;
	esp_handle.payload[0] = '\0';
}

static void ESP_State_Reset(void) {
	esp_handle.initialized = 0;
	esp_handle.rx_mode = ESP_RX_Mode_Line;
	esp_handle.mode = ESP_Mode_Unknown;
	esp_handle.wifi_state = ESP_WifiState_Unknown;
	esp_handle.tcp_receive = 0;
	esp_handle.tcp_receive_context = 0;
	ESP_NetworkDetails_Reset();
	ESP_TCPDetails_Reset();
	ESP_TX_Reset();
}

static void ESP_Line_Reset(void) {
	esp_handle.line[0] = '\0';
	esp_handle.line_length = 0;
	esp_handle.line_overflow = 0;
}

static void ESP_Command_Reset(void) {
	esp_handle.command = ESP_Command_None;
	esp_handle.command_waiting = 0;
	esp_handle.command_result = ESP_Result_Ok;
	esp_handle.response_parsed = 0;
	for (uint32_t i = 0; i < ESP_IP_OCTET_COUNT; i++) {
		esp_handle.parsed_ip[i] = 0;
	}
}

static void ESP_IPD_Reset(void) {
	esp_handle.rx_mode = ESP_RX_Mode_Line;
	ESP_Line_Reset();
	ESP_Payload_Reset();
}

static void ESP_IPD_Abort(void) {
	if (esp_handle.command_waiting) {
		esp_handle.command_result = ESP_Result_Error;
		esp_handle.command_waiting = 0;
	}

	ESP_IPD_Reset();
}

static void ESP_IPD_Timeout_Recover(void) {
	if (esp_handle.rx_mode != ESP_RX_Mode_IPDPayload) return;
	if ((uint32_t)(esp_millis - esp_handle.ipd_started_at) < ESP_IPD_TIMEOUT_MS) return;

	ESP_IPD_Abort();
}

static void ESP_TX_Timeout_Recover(void) {
	if (esp_handle.tx_state == ESP_TX_State_Idle) return;
	if ((uint32_t)(esp_millis - esp_handle.tx_started_at) < ESP_TX_TIMEOUT_MS) return;

	ESP_TX_Reset();
}

static uint8_t ESP_ConfigText_IsValid(const char* text, uint32_t max_length, uint8_t allow_empty) {
	if (text == 0) return 0;

	uint32_t length = 0;
	while (text[length] != '\0') {
		if (length >= max_length) return 0;
		if (text[length] == '"' || text[length] == '\\' || text[length] == '\r' || text[length] == '\n') return 0;
		length++;
	}

	return allow_empty || length > 0U;
}

static uint8_t ESP_Config_IsValid(const ESP_Config* config) {
	return config != 0 &&
		config->usart != 0 &&
		config->server_port != 0U &&
		ESP_ConfigText_IsValid(config->ssid, ESP_SSID_MAX_LENGTH, 0) &&
		ESP_ConfigText_IsValid(config->password, ESP_PASSWORD_MAX_LENGTH, 1);
}

static uint8_t ESP_Format_IsValid(int length, uint32_t capacity) {
	return length > 0 && (uint32_t)length < capacity;
}

static uint8_t ESP_IP_IsZero(const uint8_t ip[ESP_IP_OCTET_COUNT]) {
	for (uint32_t i = 0; i < ESP_IP_OCTET_COUNT; i++) {
		if (ip[i] != 0) return 0;
	}
	return 1;
}

static uint8_t ESP_IP_Parse(const char* input, uint8_t ip[ESP_IP_OCTET_COUNT]) {
	if (input == 0 || *input++ != '\"') return 0;

	for (uint32_t i = 0; i < ESP_IP_OCTET_COUNT; i++) {
		uint32_t octet;
		if (!STRING_ParseUnsigned(&input, &octet) || octet > 255U) return 0;
		ip[i] = (uint8_t)octet;

		char delimiter = i == ESP_IP_OCTET_COUNT - 1U ? '\"' : '.';
		if (*input++ != delimiter) return 0;
	}

	return *input == '\0';
}

static void ESP_Line_Process(void) {
	esp_handle.line[esp_handle.line_length] = '\0';
	const char* line = esp_handle.line;
	uint8_t tx_failed = STRING_Equals(line, "SEND FAIL") ||
		STRING_Equals(line, "ERROR") ||
		STRING_Equals(line, "FAIL") ||
		STRING_StartsWith(line, "busy") ||
		STRING_StartsWith(line, "link is not valid");

	if (esp_handle.tx_state == ESP_TX_State_WaitSendResult && STRING_Equals(line, "SEND OK")) {
		ESP_TX_Reset();
	}
	else if (esp_handle.tx_state != ESP_TX_State_Idle && tx_failed) {
		ESP_TX_Reset();
	}

	if (STRING_Equals(line, "WIFI CONNECTED")) {
		esp_handle.wifi_state = ESP_WifiState_Connected;
		ESP_NetworkDetails_Reset();
	}
	else if (STRING_Equals(line, "WIFI GOT IP")) {
		esp_handle.wifi_state = ESP_WifiState_GotIP;
		ESP_IP_Reset();
	}
	else if (STRING_Equals(line, "WIFI DISCONNECT") || STRING_Equals(line, "WIFI DISCONNECTED")) {
		esp_handle.wifi_state = ESP_WifiState_Disconnected;
		ESP_NetworkDetails_Reset();
		ESP_TCPConnection_Reset();
	}
	else if (esp_handle.command == ESP_Command_CIFSR &&
		STRING_StartsWith(line, "+CIFSR:STAIP,") &&
		ESP_IP_Parse(line + 13, esp_handle.parsed_ip)) {
		esp_handle.response_parsed = 1;
	}
	else if (esp_handle.command == ESP_Command_CIPSTATUS && STRING_StartsWith(line, "STATUS:")) {
		const char* value = line + 7;
		uint32_t status;
		if (STRING_ParseUnsigned(&value, &status) && *value == '\0') {
			if (status == 3U) esp_handle.tcp_state = ESP_TCPState_Connected;
			else ESP_TCPConnection_Reset();
		}
	}
	else if (esp_handle.command == ESP_Command_CIPSTATUS && STRING_StartsWith(line, "+CIPSTATUS:")) {
		const char* value = line + 11;
		uint32_t connection_id;
		if (STRING_ParseUnsigned(&value, &connection_id) && connection_id <= 4U && *value == ',') {
			if (esp_handle.active_connection_id != (uint8_t)connection_id) connection_generation++;
			esp_handle.active_connection_id = (uint8_t)connection_id;
			esp_handle.tcp_state = ESP_TCPState_Connected;
		}
	}
	else if (esp_handle.line_length > 2 && line[0] >= '0' && line[0] <= '4' && line[1] == ',' && STRING_Equals(&line[2], "CONNECT")) {
		/* The server admits one client; never let another ID take over its session. */
		uint8_t connection_id = (uint8_t)(line[0] - '0');
		if (esp_handle.tcp_state != ESP_TCPState_Connected || esp_handle.active_connection_id == connection_id) {
			connection_generation++;
			ESP_TX_Reset();
			esp_handle.active_connection_id = connection_id;
			esp_handle.tcp_state = ESP_TCPState_Connected;
		}
	}
	else if (esp_handle.line_length > 2 && line[0] >= '0' && line[0] <= '4' && line[1] == ',' && STRING_Equals(&line[2], "CLOSED")) {
		if (esp_handle.active_connection_id == (uint8_t)(line[0] - '0')) ESP_TCPConnection_Reset();
	}

	if (esp_handle.command_waiting &&
		(STRING_Equals(line, "OK") || STRING_Equals(line, "ERROR") || STRING_Equals(line, "FAIL"))) {
		esp_handle.command_result = STRING_Equals(line, "OK") ? ESP_Result_Ok : ESP_Result_Error;
		esp_handle.command_waiting = 0;
	}

	ESP_Line_Reset();
}

static uint8_t ESP_TCPMetadata_Process(void) {
	esp_handle.line[esp_handle.line_length] = '\0';
	const char* cursor = esp_handle.line;
	uint32_t connection_id;
	uint32_t payload_length;

	if (!STRING_ParseUnsigned(&cursor, &connection_id) || *cursor++ != ',') return 0;
	if (!STRING_ParseUnsigned(&cursor, &payload_length) || *cursor != '\0' || payload_length == 0U) return 0;

	esp_handle.rx_connection_id = connection_id <= 4U ? (uint8_t)connection_id : ESP_CONNECTION_ID_INVALID;
	esp_handle.payload_length = payload_length;
	esp_handle.payload_discard = connection_id > 4U || connection_id != esp_handle.active_connection_id;
	esp_handle.metadata_parsed = 1;
	ESP_Line_Reset();
	return 1;
}

static void ESP_TCPPayload_Process(void) {
	if (esp_handle.payload_buffer_length != 0U && esp_handle.tcp_receive != 0) {
		esp_handle.tcp_receive(esp_handle.rx_connection_id, esp_handle.payload,
			esp_handle.payload_buffer_length, esp_handle.tcp_receive_context);
	}
	esp_handle.payload_buffer_length = 0;
}

void ESP_Update(void) {
	if (esp_handle.config.usart == 0) return;
	ESP_IPD_Timeout_Recover();
	ESP_TX_Timeout_Recover();

	char c;
	while (USART_Receive_Char(esp_handle.config.usart, &c) == 0) {
		if (esp_handle.rx_mode == ESP_RX_Mode_IPDPayload) {
			if (!esp_handle.metadata_parsed) {
				if (c == ':') {
					if (esp_handle.line_overflow || !ESP_TCPMetadata_Process()) ESP_IPD_Abort();
					continue;
				}

				if (esp_handle.line_overflow) continue;
				if (esp_handle.line_length >= ESP_LINE_BUFFER_SIZE - 1U) {
					esp_handle.line_overflow = 1;
					continue;
				}

				esp_handle.line[esp_handle.line_length++] = c;
				esp_handle.line[esp_handle.line_length] = '\0';
			}
			else {
				if (!esp_handle.payload_discard) {
					esp_handle.payload[esp_handle.payload_buffer_length++] = c;
				}
				esp_handle.payload_position++;

				if (esp_handle.payload_buffer_length == ESP_PAYLOAD_BUFFER_SIZE ||
					esp_handle.payload_position >= esp_handle.payload_length) ESP_TCPPayload_Process();
				if (esp_handle.payload_position >= esp_handle.payload_length) {
					ESP_IPD_Reset();
				}
			}
			continue;
		}

		if (esp_handle.tx_prompt_space_pending) {
			esp_handle.tx_prompt_space_pending = 0;
			if (c == ' ') continue;
		}

		if (esp_handle.tx_state == ESP_TX_State_WaitPrompt && esp_handle.line_length == 0U && !esp_handle.line_overflow && c == '>') {
			esp_handle.tx_state = ESP_TX_State_WaitSendResult;
			esp_handle.tx_started_at = esp_millis;
			esp_handle.tx_prompt_space_pending = 1;
			USART_Transmit(esp_handle.config.usart, esp_handle.tx_data, esp_handle.tx_length);
			continue;
		}

		if (c == '\r') continue;
		if (c == '\n') {
			if (esp_handle.line_overflow) {
				ESP_Line_Reset();
			}
			else if (esp_handle.line_length != 0U) {
				ESP_Line_Process();
			}
			continue;
		}

		if (esp_handle.line_overflow) continue;
		if (esp_handle.line_length >= ESP_LINE_BUFFER_SIZE - 1U) {
			esp_handle.line_overflow = 1;
			continue;
		}

		esp_handle.line[esp_handle.line_length++] = c;
		esp_handle.line[esp_handle.line_length] = '\0';

		if (STRING_StartsWith(esp_handle.line, "+IPD,")) {
			esp_handle.rx_mode = ESP_RX_Mode_IPDPayload;
			esp_handle.ipd_started_at = esp_millis;
			ESP_Line_Reset();
		}
	}

	ESP_IPD_Timeout_Recover();
	ESP_TX_Timeout_Recover();
}

static ESP_Result ESP_Command_Execute(ESP_Command command, const char* command_text, uint32_t timeout_ms) {
	if (esp_handle.config.usart == 0) return ESP_Result_InvalidConfig;
	if (command_text == 0 || command_text[0] == '\0' || timeout_ms == 0U) return ESP_Result_InvalidCommand;

	for (const char* c = command_text; *c != '\0'; c++) {
		if (*c == '\r' || *c == '\n') return ESP_Result_InvalidCommand;
	}

	ESP_Update();
	if (esp_handle.tx_state != ESP_TX_State_Idle || esp_handle.command_waiting) return ESP_Result_Busy;
	if (esp_handle.rx_mode != ESP_RX_Mode_Line || esp_handle.line_length != 0U || esp_handle.line_overflow) {
		return ESP_Result_Error;
	}
	ESP_Line_Reset();
	esp_handle.command = command;
	esp_handle.response_parsed = 0;
	esp_handle.command_result = ESP_Result_Timeout;
	esp_handle.command_waiting = 1;

	USART_Transmit_String(esp_handle.config.usart, command_text);
	USART_Transmit_String(esp_handle.config.usart, "\r\n");

	uint32_t started_at = esp_millis;
	while (esp_handle.command_waiting) {
		ESP_Update();
		if ((uint32_t)(esp_millis - started_at) >= timeout_ms) {
			esp_handle.command_result = ESP_Result_Timeout;
			esp_handle.command_waiting = 0;
		}
	}

	esp_handle.command = ESP_Command_None;
	return esp_handle.command_result;
}

ESP_Result ESP_Init(const ESP_Config* config) {
	esp_handle.config.usart = 0;
	esp_handle.config.ssid = 0;
	esp_handle.config.password = 0;
	esp_handle.config.server_port = 0;
	ESP_State_Reset();
	ESP_Line_Reset();
	ESP_Payload_Reset();
	ESP_Command_Reset();
	if (!ESP_Config_IsValid(config)) return ESP_Result_InvalidConfig;

	esp_handle.config = *config;

	ESP_Result result = ESP_Command_Execute(ESP_Command_AT, "AT", ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;

	result = ESP_Command_Execute(ESP_Command_CWMODE, "AT+CWMODE=1", ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;
	esp_handle.mode = ESP_Mode_Station;

	char wifi_command[ESP_WIFI_COMMAND_BUFFER_SIZE];
	int wifi_command_length = STRING_snprintf(
		wifi_command,
		ESP_WIFI_COMMAND_BUFFER_SIZE,
		"AT+CWJAP=\"%s\",\"%s\"",
		esp_handle.config.ssid,
		esp_handle.config.password
	);
	if (!ESP_Format_IsValid(wifi_command_length, ESP_WIFI_COMMAND_BUFFER_SIZE)) return ESP_Result_InvalidConfig;

	result = ESP_Command_Execute(ESP_Command_CWJAP, wifi_command, ESP_JOIN_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;

	if (esp_handle.wifi_state != ESP_WifiState_GotIP) {
		esp_handle.wifi_state = ESP_WifiState_Connected;
	}

	result = ESP_Command_Execute(ESP_Command_CIFSR, "AT+CIFSR", ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;
	if (!esp_handle.response_parsed || ESP_IP_IsZero(esp_handle.parsed_ip)) {
		ESP_IP_Reset();
		if (esp_handle.wifi_state == ESP_WifiState_GotIP) {
			esp_handle.wifi_state = ESP_WifiState_Connected;
		}
		return ESP_Result_Error;
	}

	result = ESP_Command_Execute(ESP_Command_CIPMUX, "AT+CIPMUX=1", ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;
	esp_handle.connection_mode = ESP_ConnectionMode_Multiple;

	/* ESP-AT servers require CIPMUX=1 even when only one client is supported. */
	result = ESP_Command_Execute(ESP_Command_CIPSERVERMAXCONN, "AT+CIPSERVERMAXCONN=1", ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;

	char server_command[ESP_COMMAND_BUFFER_SIZE];
	int server_command_length = STRING_snprintf(
		server_command,
		ESP_COMMAND_BUFFER_SIZE,
		"AT+CIPSERVER=1,%u",
		(unsigned int)esp_handle.config.server_port
	);
	if (!ESP_Format_IsValid(server_command_length, ESP_COMMAND_BUFFER_SIZE)) return ESP_Result_InvalidConfig;

	result = ESP_Command_Execute(ESP_Command_CIPSERVER, server_command, ESP_TIMEOUT_MS);
	if (result != ESP_Result_Ok) return result;
	esp_handle.server_state = ESP_ServerState_Started;

	for (uint32_t i = 0; i < ESP_IP_OCTET_COUNT; i++) {
		esp_handle.ip[i] = esp_handle.parsed_ip[i];
	}
	esp_handle.wifi_state = ESP_WifiState_GotIP;
	esp_handle.initialized = 1;
	return ESP_Result_Ok;
}

ESP_Mode ESP_Mode_Get(void) {
	return esp_handle.mode;
}

ESP_WifiState ESP_WifiState_Get(void) {
	return esp_handle.wifi_state;
}

const char* ESP_SSID_Get(void) {
	return esp_handle.config.ssid != 0 && esp_handle.config.ssid[0] != '\0' ? esp_handle.config.ssid : 0;
}

const uint8_t* ESP_IP_Get(void) {
	return !ESP_IP_IsZero(esp_handle.ip) ? esp_handle.ip : 0;
}

ESP_TCPState ESP_TCPState_Get(void) {
	return esp_handle.tcp_state;
}

uint8_t ESP_ConnectionID_Get(void) {
	return esp_handle.active_connection_id;
}

uint32_t ESP_ConnectionGeneration_Get(void) {
	return connection_generation;
}

ESP_ConnectionMode ESP_ConnectionMode_Get(void) {
	return esp_handle.connection_mode;
}

ESP_ServerState ESP_ServerState_Get(void) {
	return esp_handle.server_state;
}

ESP_Result ESP_TCPStatus_Query(void) {
	return ESP_Command_Execute(ESP_Command_CIPSTATUS, "AT+CIPSTATUS", ESP_TIMEOUT_MS);
}

void ESP_TCPReceiveCallback_Set(ESP_TCPReceiveFn callback, void* context) {
	if (callback == 0) return;
	esp_handle.tcp_receive = callback;
	esp_handle.tcp_receive_context = context;
}

uint8_t ESP_TCPSend_IsBusy(void) {
	return esp_handle.tx_state != ESP_TX_State_Idle;
}

ESP_Result ESP_TCPSend(uint8_t connection_id, const char* data, uint32_t length) {
	if (!esp_handle.initialized || esp_handle.config.usart == 0) return ESP_Result_InvalidConfig;
	if (data == 0 || length == 0U) return ESP_Result_InvalidCommand;
	if (length > ESP_TX_BUFFER_SIZE) return ESP_Result_Capacity;

	uint32_t generation = connection_generation;
	ESP_Update();
	if (generation != connection_generation) return ESP_Result_Disconnected;
	if (esp_handle.tx_state != ESP_TX_State_Idle || esp_handle.command_waiting) return ESP_Result_Busy;
	if (esp_handle.tcp_state != ESP_TCPState_Connected || connection_id != esp_handle.active_connection_id) {
		return ESP_Result_Disconnected;
	}

	char command[ESP_COMMAND_BUFFER_SIZE];
	int command_length = STRING_snprintf(
		command,
		ESP_COMMAND_BUFFER_SIZE,
		"AT+CIPSEND=%u,%u\r\n",
		(unsigned int)connection_id,
		(unsigned int)length
	);
	if (!ESP_Format_IsValid(command_length, ESP_COMMAND_BUFFER_SIZE)) return ESP_Result_Error;

	for (uint32_t i = 0; i < length; i++) {
		esp_handle.tx_data[i] = data[i];
	}

	esp_handle.tx_connection_id = connection_id;
	esp_handle.tx_length = length;
	esp_handle.tx_started_at = esp_millis;
	esp_handle.tx_state = ESP_TX_State_WaitPrompt;
	USART_Transmit(esp_handle.config.usart, command, (uint32_t)command_length);
	return ESP_Result_Ok;
}

ESP_Result ESP_TestCommand_Execute(const char* command) {
	return ESP_Command_Execute(ESP_Command_Test, command, ESP_JOIN_TIMEOUT_MS);
}
