#include "../drivers/inc/gpio.h"
#include "../drivers/inc/usart.h"

int main(void) {

	RCC_GPIOCClock_Enable();

	GPIO_Config button = {
		.port = GPIOC,
		.pin = GPIO_Pin_13,
		.mode = GPIO_Mode_Input,
		.output_type = GPIO_Output_PushPull,
		.speed = GPIO_Speed_Low,
		.pull = GPIO_Pull_Up,
	};

	GPIO_Init(&button);

	USART2_Init();

	USART_Transmit_Char(USART2, 'H');
	USART_Transmit_Char(USART2, 'I');
	USART_Transmit_Char(USART2, '?');

	while (1) {
		if (GPIO_ReadPin(GPIOC, GPIO_Pin_13) == 0) {
			USART_Transmit_Char(USART2, 'H');
			USART_Transmit_Char(USART2, 'I');
			USART_Transmit_Char(USART2, '?');

			while (GPIO_ReadPin(GPIOC, GPIO_Pin_13) == 0) {
			}
		}
	}

	return 0;
}
