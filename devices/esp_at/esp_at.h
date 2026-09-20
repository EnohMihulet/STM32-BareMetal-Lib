#pragma once

#include "usart.h"

typedef struct {
	USART_TypeDef *usart;
	const char *ssid;
	const char *password;
	uint16_t server_port;
} ESP_Config;

typedef enum {
	ESP_Result_Ok = 0,
	ESP_Result_Error,
	ESP_Result_Timeout,
	ESP_Result_Busy,
	ESP_Result_Capacity,
	ESP_Result_Disconnected,
	ESP_Result_InvalidConfig,
	ESP_Result_InvalidCommand,
} ESP_Result;

typedef enum {
	ESP_Mode_Unknown = -1,
	ESP_Mode_Null = 0,
	ESP_Mode_Station = 1,
	ESP_Mode_SoftAP = 2,
	ESP_Mode_StationSoftAP = 3,
} ESP_Mode;

typedef enum {
	ESP_WifiState_Unknown,
	ESP_WifiState_Disconnected,
	ESP_WifiState_Connected,
	ESP_WifiState_GotIP,
} ESP_WifiState;

typedef enum {
    ESP_TCPState_Disconnected,
    ESP_TCPState_Connected,
} ESP_TCPState;

typedef enum {
    ESP_ConnectionMode_Single,
    ESP_ConnectionMode_Multiple,
} ESP_ConnectionMode;

typedef enum {
    ESP_ServerState_NotStarted,
    ESP_ServerState_Started,
} ESP_ServerState;

/* Payloads are delivered in bounded chunks, not application message boundaries. */
typedef void (*ESP_TCPReceiveFn)(uint8_t connection_id, const char* data, uint32_t length, void* context);

ESP_Result ESP_Init(const ESP_Config* config);
void ESP_Tick(void);

ESP_Mode ESP_Mode_Get(void);
ESP_WifiState ESP_WifiState_Get(void);
const char* ESP_SSID_Get(void);
const uint8_t* ESP_IP_Get(void);

ESP_TCPState ESP_TCPState_Get(void);
uint8_t ESP_ConnectionID_Get(void);
/* Changes on disconnect/reconnect, including reuse of the same connection ID. */
uint32_t ESP_ConnectionGeneration_Get(void);
ESP_ConnectionMode ESP_ConnectionMode_Get(void);
ESP_ServerState ESP_ServerState_Get(void);
ESP_Result ESP_TCPStatus_Query(void);

void ESP_TCPReceiveCallback_Set(ESP_TCPReceiveFn callback, void* context);

ESP_Result ESP_TCPSend(uint8_t connection_id, const char* data, uint32_t length);

uint8_t ESP_TCPSend_IsBusy(void);

ESP_Result ESP_TestCommand_Execute(const char* command);
void ESP_Update(void);
