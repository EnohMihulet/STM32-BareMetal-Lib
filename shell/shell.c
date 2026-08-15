#include "shell.h"
#include "../drivers/inc/usart.h"

#define SHELL_INPUT_BUFFER_SIZE 64
#define SHELL_CHAR_BACKSPACE '\b'
#define SHELL_CHAR_DELETE 0x7f

static char input_buffer[SHELL_INPUT_BUFFER_SIZE];
static const char* argv[SHELL_MAX_TOKENS];
static uint32_t input_index;

static const SHELL_Command* shell_commands;
static uint32_t shell_command_count;

#define X(RESULT, MESSAGE) MESSAGE,
static const char* const SHELL_ResultMessages[SHELL_RESULT_COUNT] = {
	SHELL_RESULTS
};
#undef X


static uint8_t SHELL_StringEquals(const char* a, const char* b) {
	while (*a != '\0' && *b != '\0') {
		if (*a != *b) {
			return 0;
		}

		a++;
		b++;
	}

	return *a == *b;
}

void SHELL_Write(const char* s) {
	USART_Transmit_String(USART2, s);
}

void SHELL_WriteChar(const char c) {
	USART_Transmit_Char(USART2, c);
}

void SHELL_Commands_Set(const SHELL_Command* commands, uint32_t command_count) {
	shell_commands = commands;
	shell_command_count = command_count;
}

static void SHELL_PrintResult(SHELL_Result result) {
	if ((uint32_t)result >= SHELL_RESULT_COUNT) {
		result = SHELL_RESULT_INTERNAL;
	}

	SHELL_Write(SHELL_ResultMessages[result]);
	SHELL_Write("\r\n");
}

void SHELL_PrintCommand(const SHELL_Command* command) {
	SHELL_Write(command->name);
	SHELL_Write(": ");
	SHELL_Write(command->help);
	SHELL_Write("\r\n");
}

void SHELL_PrintCommandList(void) {
	for (uint32_t i = 0; i < shell_command_count; i++) {
		SHELL_PrintCommand(&shell_commands[i]);
	}
}

static SHELL_Result SHELL_ParseArgs(char* input, const char* argv[], uint32_t max_args, uint32_t* argc_out) {
	uint32_t argc = 0;
	char* c = input;

	while (*c != '\0') {
		while (*c == ' ') {
			*c = '\0';
			c++;
		}

		if (*c == '\0') {
			break;
		}

		if (argc >= max_args) {
			*argc_out = argc;
			return SHELL_RESULT_CAPACITY;
		}

		argv[argc] = c;
		argc++;

		while (*c != '\0' && *c != ' ') {
			c++;
		}
	}

	*argc_out = argc;
	return SHELL_RESULT_OK;
}

const SHELL_Command* SHELL_LookupCommand(const char* name) {
	for (uint32_t i = 0; i < shell_command_count; i++) {
		if (SHELL_StringEquals(name, shell_commands[i].name)) {
			return &shell_commands[i];
		}
	}

	return 0;
}

static SHELL_Result SHELL_DispatchCommand(const SHELL_Command* command, uint32_t argc, const char* argv[]) {
	if (command == 0 || argv == 0 || argc == 0) {
		return SHELL_RESULT_INTERNAL;
	}

	uint32_t command_argc = argc - 1;

	if (command_argc < command->min_args || command_argc > command->max_args) {
		return SHELL_RESULT_ARGUMENT_COUNT;
	}

	return command->handler((int)command_argc, &argv[1]);
}

void SHELL_Init() {
	for (uint32_t i = 0; i < SHELL_INPUT_BUFFER_SIZE; i++) input_buffer[i] = 0;
	for (uint32_t i = 0; i < SHELL_MAX_TOKENS; i++) argv[i] = 0;
	input_index = 0;
	SHELL_Write("> ");
}

void SHELL_Update() {
	if (USART_Receive_Char(USART2, &input_buffer[input_index]) != 0) return;

	if (input_buffer[input_index] == SHELL_CHAR_BACKSPACE || input_buffer[input_index] == SHELL_CHAR_DELETE) {
		if (input_index > 0) {
			input_index -= 1;
			SHELL_Write("\b \b");
		}
		return;
	}

	SHELL_WriteChar(input_buffer[input_index]);

	if (input_buffer[input_index] != '\r') {
		if (input_index == SHELL_INPUT_BUFFER_SIZE - 1) {
			SHELL_PrintResult(SHELL_INPUT_BUFFER_SIZE);
			SHELL_Write("> ");
			input_index = 0;
		}
		input_index += 1;
		return;
	}

	input_buffer[input_index] = '\0';
	SHELL_WriteChar('\n');

	uint32_t argc = 0;
	SHELL_Result parse_result = SHELL_ParseArgs(input_buffer, argv, SHELL_MAX_TOKENS, &argc);
	if (parse_result != SHELL_RESULT_OK) {
		SHELL_PrintResult(parse_result);
		SHELL_Write("> ");
		input_index = 0;
		return;
	}

	if (argc == 0) {
		SHELL_Write("> ");
		input_index = 0;
		return;
	}
	
	const SHELL_Command* command = SHELL_LookupCommand(argv[0]);
	if (command == 0) {
		SHELL_PrintResult(SHELL_RESULT_UNKNOWN_COMMAND);
		SHELL_Write("> ");
		input_index = 0;
		return;
	}

	SHELL_Result result = SHELL_DispatchCommand(command, argc, argv);
	if (result != SHELL_RESULT_OK) {
		SHELL_PrintResult(result);
	}

	SHELL_Write("> ");
	input_index = 0;
}
