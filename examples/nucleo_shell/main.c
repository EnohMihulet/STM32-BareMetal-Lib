#include "board_button.h"
#include "board_led.h"
#include "example_shell_commands.h"
#include "shell.h"
#include "usart.h"

int main(void) {
	Board_Button_Init();
	Board_LED_Init();
	USART2_Init();
	Example_ShellCommands_Init();
	SHELL_Start();

	while (1) {
	}

	return 0;
}
