#include "../drivers/inc/pwm.h"
#include "../drivers/inc/rcc.h"
#include "../drivers/inc/usart.h"
#include "board_led.h"
#include "../shell/shell.h"

int main(void) {
	PWM_ChannelConfig led_pwm = {
		.tim = TIM2,
		.channel = TIM_Channel_1,
		.timer_clock_hz = 16000000,
		.frequency_hz = 1000,
		.duty_per_mille = 500,
		.polarity = TIM_OutputPolarity_ActiveHigh,
	};

	Board_LED_PWM_Init();
	RCC_TIM2Clock_Enable();
	PWM_Channel_Init(&led_pwm);
	USART2_Init();
	SHELL_Start();
	
	while (1) {
	}

	return 0;
}
