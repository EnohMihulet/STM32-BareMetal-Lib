#include "shell.h"
#include "../app/board_button.h"
#include "../app/board_led.h"

static uint8_t SHELL_CommandStringEquals(const char* a, const char* b) {
	while (*a != '\0' && *b != '\0') {
		if (*a != *b) {
			return 0;
		}

		a++;
		b++;
	}

	return *a == *b;
}

SHELL_Result SHELL_CommandHelp(int argc, const char* argv[]) {
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

SHELL_Result SHELL_CommandLed(int argc, const char* argv[]) {
	(void)argc;

	if (SHELL_CommandStringEquals(argv[0], "on")) {
		LED_On();
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "off")) {
		LED_Off();
		return SHELL_RESULT_OK;
	}

	return SHELL_RESULT_BAD_ARGUMENT;
}

SHELL_Result SHELL_CommandButton(int argc, const char* argv[]) {
	(void)argc;
	(void)argv;

	if (Board_Button_IsPressed()) {
		SHELL_Write("Button: pressed\r\n");
	}
	else {
		SHELL_Write("Button: released\r\n");
	}

	return SHELL_RESULT_OK;
}
