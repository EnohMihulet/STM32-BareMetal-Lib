#include "example_shell_commands.h"

#include "board_button.h"
#include "board_led.h"
#include "shell.h"

static uint8_t Example_CommandStringEquals(const char* a, const char* b) {
	while (*a != '\0' && *b != '\0') {
		if (*a != *b) {
			return 0;
		}

		a++;
		b++;
	}

	return *a == *b;
}

static SHELL_Result Example_CommandHelp(int argc, const char* argv[]) {
	if (argc == 0) {
		(void)argv;

		SHELL_PrintCommandList();
		return SHELL_RESULT_OK;
	}

	const SHELL_Command* command = SHELL_LookupCommand(argv[0]);
	if (command == 0) {
		return SHELL_RESULT_UNKNOWN_COMMAND;
	}

	SHELL_PrintCommand(command);
	return SHELL_RESULT_OK;
}

static SHELL_Result Example_CommandLed(int argc, const char* argv[]) {
	(void)argc;

	if (Example_CommandStringEquals(argv[0], "on")) {
		LED_On();
		return SHELL_RESULT_OK;
	}

	if (Example_CommandStringEquals(argv[0], "off")) {
		LED_Off();
		return SHELL_RESULT_OK;
	}

	if (Example_CommandStringEquals(argv[0], "toggle")) {
		LED_Toggle();
		return SHELL_RESULT_OK;
	}

	return SHELL_RESULT_BAD_ARGUMENT;
}

static SHELL_Result Example_CommandButton(int argc, const char* argv[]) {
	(void)argc;
	(void)argv;

	SHELL_Write("Button: ");
	SHELL_Write(Board_Button_IsPressed() ? "pressed" : "released");
	SHELL_Write("\r\n");
	return SHELL_RESULT_OK;
}

static const SHELL_Command example_commands[] = {
	{"help", Example_CommandHelp, 0, 1, "List available commands"},
	{"led", Example_CommandLed, 1, 1, "Set onboard LED: led on|off|toggle"},
	{"button", Example_CommandButton, 0, 0, "Read onboard button state"},
};

void Example_ShellCommands_Init(void) {
	SHELL_Commands_Set(example_commands, (uint32_t)(sizeof(example_commands) / sizeof(example_commands[0])));
}
