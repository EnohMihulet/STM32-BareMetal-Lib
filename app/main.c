#include "../drivers/inc/usart.h"
#include "board_button.h"
#include "board_led.h"
#include "../shell/shell.h"

int main(void) {

	Board_Button_Init();
	Board_LED_Init();

	USART2_Init();
	SHELL_Start();

	return 0;
}
