#pragma once
#include <stdint.h>

#define SHELL_MAX_TOKENS 8

#define SHELL_RESULTS \
	X(SHELL_RESULT_OK,                "OK") \
	X(SHELL_RESULT_UNKNOWN_COMMAND,   "Unknown command. Type 'help' to list available commands.") \
	X(SHELL_RESULT_ARGUMENT_COUNT,    "Wrong number of arguments. Type 'help' to check command usage.") \
	X(SHELL_RESULT_BAD_NUMBER,        "Expected a numeric argument, but the value could not be parsed.") \
	X(SHELL_RESULT_OUT_OF_RANGE,      "Argument is outside the allowed range.") \
	X(SHELL_RESULT_BAD_ARGUMENT,      "Invalid argument value. Type 'help' to check command usage.") \
	X(SHELL_RESULT_INVALID_STATE,     "Command is not allowed in the current state.") \
	X(SHELL_RESULT_CAPACITY,          "Input has too many arguments for the shell buffer.") \
	X(SHELL_RESULT_EMPTY,             "Required argument or value is missing.") \
	X(SHELL_RESULT_BUSY,              "Shell is busy. Try again later.") \
	X(SHELL_RESULT_INTERNAL,          "Unexpected internal shell failure.")

#define X(RESULT, MESSAGE) RESULT,
typedef enum SHELL_Result {
	SHELL_RESULTS
	SHELL_RESULT_COUNT
} SHELL_Result;
#undef X

typedef void (*SHELL_WriteFn)(const char* data, uint32_t length, void* context);

typedef struct {
	SHELL_WriteFn write;
	void* context;
} SHELL_Output;

typedef SHELL_Result (*SHELL_CommandHandler)(const SHELL_Output* output, int argc, const char* argv[]);

typedef struct SHELL_Command {
	const char* name;
	SHELL_CommandHandler handler;
	uint8_t min_args;
	uint8_t max_args;
	const char* help;
} SHELL_Command;

typedef struct {
	SHELL_Output output;
	char *data;
	uint32_t length;
	uint32_t capacity;
	uint8_t overflow;
} SHELL_Buffer;

void SHELL_PrintResult(const SHELL_Output* output, SHELL_Result result);

void SHELL_Commands_Set(const SHELL_Command* commands, uint32_t command_count);
void SHELL_PrintCommand(const SHELL_Output* output, const SHELL_Command* command);
void SHELL_PrintCommandList(const SHELL_Output* output);

const SHELL_Command* SHELL_LookupCommand(const char* name);

SHELL_Result SHELL_ExecuteCommand(char* command, const SHELL_Output* output);

void SHELL_Write(const SHELL_Output *output, const char *text);
void SHELL_WriteChar(const SHELL_Output *output, char value);
void SHELL_WriteUnsigned(const SHELL_Output *output, uint32_t value);

void SHELL_Buffer_Init(SHELL_Buffer* buffer, char* storage, uint32_t capacity);
const SHELL_Output* SHELL_Buffer_Output(SHELL_Buffer* buffer);
void SHELL_Buffer_Reset(SHELL_Buffer* buffer);

void SHELL_Prompt_Defer(void);
void SHELL_Prompt_Write(void);

void SHELL_Init();
void SHELL_Update();
