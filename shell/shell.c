#include "shell.h"
#include "../drivers/inc/usart.h"
#include "../core/string_helper.h"
#include <stddef.h>

#define SHELL_INPUT_BUFFER_SIZE 64
#define SHELL_CHAR_BACKSPACE '\b'
#define SHELL_CHAR_DELETE 0x7f

static char input_buffer[SHELL_INPUT_BUFFER_SIZE];
static const char* argv[SHELL_MAX_TOKENS];
static uint32_t input_index;

static const SHELL_Command* shell_commands;
static uint32_t shell_command_count;
static uint8_t prompt_deferred;

#define X(RESULT, MESSAGE) MESSAGE,
static const char* const SHELL_ResultMessages[SHELL_RESULT_COUNT] = {
	SHELL_RESULTS
};
#undef X

static void SHELL_USARTWrite(const char* s, uint32_t length, void* context) {
	USART_TypeDef* usart = context;
	USART_Transmit(usart, s, length);
}

static void SHELL_BufferWrite(const char* s, uint32_t length, void* context) {
	SHELL_Buffer* buffer = context;
	if (buffer == NULL || s == NULL || length == 0U) return;
	if (buffer->overflow) return;

	if (buffer->data == NULL || buffer->capacity == 0U || buffer->length >= buffer->capacity ||
		length > buffer->capacity - buffer->length - 1U) {
		buffer->overflow = 1;
		return;
	}

	for (uint32_t i = 0; i < length; i++) {
		buffer->data[buffer->length + i] = s[i];
	}
	buffer->length += length;
	buffer->data[buffer->length] = '\0';
}

static SHELL_Output shell_usart_output = {
	.write= &SHELL_USARTWrite,
	.context = USART2
};

void SHELL_Buffer_Init(SHELL_Buffer* buffer, char* storage, uint32_t capacity) {
	if (buffer == NULL) return;

	buffer->output.write = &SHELL_BufferWrite;
	buffer->output.context = buffer;
	buffer->data = storage;
	buffer->capacity = storage != NULL ? capacity : 0U;
	SHELL_Buffer_Reset(buffer);
}

const SHELL_Output* SHELL_Buffer_Output(SHELL_Buffer* buffer) {
	if (buffer == NULL || buffer->data == NULL || buffer->capacity == 0U) return NULL;
	return &buffer->output;
}

void SHELL_Write(const SHELL_Output *output, const char *text) {
	if (output != NULL && text != NULL && output->write != NULL) {
		uint32_t length = STRING_Length(text);
		if (length == 0) return;
		output->write(text, length, output->context);
	}
}

void SHELL_WriteChar(const SHELL_Output *output, char value) {
	if (output != NULL && output->write != NULL) {
		output->write(&value, 1, output->context);
	}
}

void SHELL_WriteUnsigned(const SHELL_Output* output, uint32_t value) {
	char buffer[11];
	uint32_t index = sizeof(buffer);

	buffer[--index] = '\0';
	if (value == 0) {
		SHELL_Write(output, "0");
		return;
	}

	while (value > 0) {
		buffer[--index] = (char)('0' + (value % 10U));
		value /= 10U;
	}
	SHELL_Write(output, &buffer[index]);
}

void SHELL_Buffer_Reset(SHELL_Buffer* buffer) {
	if (buffer == NULL) return;

	buffer->length = 0;
	buffer->overflow = 0;
	if (buffer->data != NULL && buffer->capacity > 0U) {
		buffer->data[0] = '\0';
	}
}

void SHELL_Prompt_Defer(void) {
	prompt_deferred = 1;
}

void SHELL_Prompt_Write(void) {
	prompt_deferred = 0;
	SHELL_Write(&shell_usart_output, "> ");
}

void SHELL_Commands_Set(const SHELL_Command* commands, uint32_t command_count) {
	shell_commands = commands;
	shell_command_count = command_count;
}

void SHELL_PrintResult(const SHELL_Output* output, SHELL_Result result) {
	if ((uint32_t)result >= SHELL_RESULT_COUNT) {
		result = SHELL_RESULT_INTERNAL;
	}

	SHELL_Write(output, SHELL_ResultMessages[result]);
	SHELL_Write(output, "\r\n");
}

void SHELL_PrintCommand(const SHELL_Output* output, const SHELL_Command* command) {
	SHELL_Write(output, command->name);
	SHELL_Write(output, ": ");
	SHELL_Write(output, command->help);
	SHELL_Write(output, "\r\n");
}

void SHELL_PrintCommandList(const SHELL_Output* output) {
	for (uint32_t i = 0; i < shell_command_count; i++) {
		SHELL_PrintCommand(output, &shell_commands[i]);
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
		if (STRING_Equals(name, shell_commands[i].name)) {
			return &shell_commands[i];
		}
	}

	return 0;
}

static SHELL_Result SHELL_DispatchCommand(const SHELL_Output* output, const SHELL_Command* command, uint32_t argc, const char* argv[]) {
	if (command == 0 || argv == 0 || argc == 0) {
		return SHELL_RESULT_INTERNAL;
	}

	uint32_t command_argc = argc - 1;

	if (command_argc < command->min_args || command_argc > command->max_args) {
		return SHELL_RESULT_ARGUMENT_COUNT;
	}

	return command->handler(output, (int)command_argc, &argv[1]);
}

SHELL_Result SHELL_ExecuteCommand(char* command, const SHELL_Output* output) {
	if (command == 0 || output == NULL) return SHELL_RESULT_EMPTY;
	
	uint32_t argc = 0;
	SHELL_Result parse_result = SHELL_ParseArgs(command, argv, SHELL_MAX_TOKENS, &argc);
	if (parse_result != SHELL_RESULT_OK) return parse_result;
	if (argc == 0U) return SHELL_RESULT_EMPTY;

	const SHELL_Command* shell_command = SHELL_LookupCommand(argv[0]);
	if (shell_command == 0) return SHELL_RESULT_UNKNOWN_COMMAND;

	return SHELL_DispatchCommand(output, shell_command, argc, argv);
}

void SHELL_Init() {
	for (uint32_t i = 0; i < SHELL_INPUT_BUFFER_SIZE; i++) input_buffer[i] = 0;
	for (uint32_t i = 0; i < SHELL_MAX_TOKENS; i++) argv[i] = 0;
	input_index = 0;
	prompt_deferred = 0;
	SHELL_Prompt_Write();
}

void SHELL_Update() {
	if (prompt_deferred) {
		char discarded;
		while (USART_Receive_Char(USART2, &discarded) == 0) {
		}
		return;
	}

	if (USART_Receive_Char(USART2, &input_buffer[input_index]) != 0) return;

	if (input_buffer[input_index] == SHELL_CHAR_BACKSPACE || input_buffer[input_index] == SHELL_CHAR_DELETE) {
		if (input_index > 0) {
			input_index -= 1;
			SHELL_Write(&shell_usart_output, "\b \b");
		}
		return;
	}

	SHELL_WriteChar(&shell_usart_output, input_buffer[input_index]);

	if (input_buffer[input_index] != '\r') {
		if (input_index == SHELL_INPUT_BUFFER_SIZE - 1) {
			SHELL_PrintResult(&shell_usart_output, SHELL_RESULT_CAPACITY);
			if (!prompt_deferred) SHELL_Prompt_Write();
			input_index = 0;
			return;
		}
		input_index += 1;
		return;
	}

	input_buffer[input_index] = '\0';
	SHELL_WriteChar(&shell_usart_output, '\n');

	uint32_t argc = 0;
	SHELL_Result parse_result = SHELL_ParseArgs(input_buffer, argv, SHELL_MAX_TOKENS, &argc);
	if (parse_result != SHELL_RESULT_OK) {
		SHELL_PrintResult(&shell_usart_output, parse_result);
		if (!prompt_deferred) SHELL_Prompt_Write();
		input_index = 0;
		return;
	}

	if (argc == 0) {
		if (!prompt_deferred) SHELL_Prompt_Write();
		input_index = 0;
		return;
	}
	
	const SHELL_Command* command = SHELL_LookupCommand(argv[0]);
	if (command == 0) {
		SHELL_PrintResult(&shell_usart_output, SHELL_RESULT_UNKNOWN_COMMAND);
		if (!prompt_deferred) SHELL_Prompt_Write();
		input_index = 0;
		return;
	}

	SHELL_Result result = SHELL_DispatchCommand(&shell_usart_output, command, argc, argv);
	if (result != SHELL_RESULT_OK) {
		SHELL_PrintResult(&shell_usart_output, result);
	}

	if (!prompt_deferred) {
		SHELL_Prompt_Write();
	}
	input_index = 0;
}
