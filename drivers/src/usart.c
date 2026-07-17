#include "../inc/rcc.h"
#include "../inc/gpio.h"
#include "../inc/usart.h"

void USART2_Init() {
	RCC_GPIOAClock_Enable();
	RCC_USART2Clock_Enable();

	GPIO_Config usart = {
		.port = GPIOA,
		.pin = GPIO_Pin_2,
		.mode = GPIO_Mode_Alt,
		.output_type = GPIO_Output_PushPull,
		.speed = GPIO_Speed_Low,
		.pull = GPIO_Pull_None,
		.alternate_function = AF7,
	};

	GPIO_Init(&usart);

	USART_Disable(USART2);

	USART_Oversampling16_Enable(USART2);
	USART_BaudRate_Set(USART2, 115200UL);
	USART_Transmitter_Enable(USART2);
	USART_WordLength_8Bits(USART2);
	USART_Parity_Disable(USART2);
	USART_Set_StopBits(USART2, USART_One_Stop);

	USART_Enable(USART2);
}

void USART_Transmit_Char(USART_TypeDef* usart, const char c) {
	while (!MCL_READ_BIT(usart->SR, USART_SR_TXE_Bit)) {
	}

	usart->DR = c;

	while (!MCL_READ_BIT(usart->SR, USART_SR_TC_Bit)) {
	}
}
