#include "../drivers/inc/exti.h"
#include "board_button.h"
#include "board_led.h"
#include "limit_switch.h"
#include "step_output.h"
#include "../drivers/inc/usart.h"
#include "../shell/shell.h"

static void LimitSwitch_Callback(void) {
	(void)STEP_Output_Stop();
}

int main(void) {
	Board_Button_Init();
	Board_LED_Init();
	LIMIT_SWITCH_Init();
	(void)STEP_Output_Init();
	(void)STEP_Output_Start();

	EXTI_Config exti_limit_switch = {
		.port = LIMIT_SWITCH_Port_Get(),
		.pin = LIMIT_SWITCH_Pin_Get(),
		.trigger = LIMIT_SWITCH_EXTITrigger_Get(),
		.interrupt_enable = 1,
		.event_enable = 0,
	};

	(void)EXTI_Callback_Register(LIMIT_SWITCH_EXTILine_Get(), LimitSwitch_Callback);
	(void)EXTI_Line_Configure(&exti_limit_switch);
	USART2_Init();
	SHELL_Start();
	
	while (1) {
	}

	return 0;
}
