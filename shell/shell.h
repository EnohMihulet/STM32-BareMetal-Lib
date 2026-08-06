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

typedef SHELL_Result (*SHELL_CommandHandler)(int argc, const char* argv[]);

typedef struct SHELL_Command {
	const char* name;
	SHELL_CommandHandler handler;
	uint8_t min_args;
	uint8_t max_args;
	const char* help;
} SHELL_Command;

SHELL_Result SHELL_CommandHelp(int argc, const char* argv[]);
SHELL_Result SHELL_CommandLed(int argc, const char* argv[]);
SHELL_Result SHELL_CommandButton(int argc, const char* argv[]);
SHELL_Result SHELL_CommandPwm(int argc, const char* argv[]);
SHELL_Result SHELL_CommandStep(int argc, const char* argv[]);
SHELL_Result SHELL_CommandExti(int argc, const char* argv[]);
SHELL_Result SHELL_CommandSwitch(int argc, const char* argv[]);

void SHELL_PrintCommand(const SHELL_Command* command);
void SHELL_PrintCommandList(void);
const SHELL_Command* SHELL_LookupCommand(const char* name);
void SHELL_Write(const char* s);

#define SHELL_COMMANDS \
	X(help, "help", SHELL_CommandHelp, 0, 1, "List available commands") \
	X(led, "led", SHELL_CommandLed, 1, 1, "Set onboard LED: led on|off") \
	X(button, "button", SHELL_CommandButton, 0, 0, "Read onboard button state") \
	X(pwm, "pwm", SHELL_CommandPwm, 1, 2, "Test LED PWM: pwm start|stop|status|set <0-1000>|duty <0-100>") \
	X(step, "step", SHELL_CommandStep, 1, 2, "Test STEP output: step start|stop|status|freq <hz>|set <hz>") \
	X(exti, "exti", SHELL_CommandExti, 2, 2, "Test EXTI: exti swier|pending|clear <0-22>") \
	X(switch, "switch", SHELL_CommandSwitch, 0, 0, "Read switch state as active or released")

void SHELL_Start();
