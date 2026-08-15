#include "../inc/rcc.h"
#include "../inc/gpio.h"
#include "../inc/usart.h"

void USART2_Init() {
	RCC_GPIOAClock_Enable();
	RCC_USART2Clock_Enable();

	GPIO_Config usart = {
		.port = GPIOA,
		.mode = GPIO_Mode_Alt,
		.output_type = GPIO_Output_PushPull,
		.speed = GPIO_Speed_High,
		.pull = GPIO_Pull_Up,
		.alternate_function = AF7,
	};

	usart.pin = GPIO_Pin_2;
	GPIO_Init(&usart);

	usart.pin = GPIO_Pin_3;
	GPIO_Init(&usart);

	USART_Disable(USART2);

	USART_Oversampling16_Enable(USART2);
	USART_BaudRate_Set(USART2, 115200UL);
	USART_Transmitter_Enable(USART2);
	USART_Receiver_Enable(USART2);
	USART_WordLength_8Bits(USART2);
	USART_Parity_Disable(USART2);
	USART_Set_StopBits(USART2, USART_One_Stop);

	USART_Enable(USART2);
}

void USART_Transmit_Char(USART_TypeDef* usart, const char c) {
	while (!MCL_READ_BIT(usart->SR, USART_SR_TXE_Bit)) {
	}

	usart->DR = ((uint32_t)c & 0xFFU);

	while (!MCL_READ_BIT(usart->SR, USART_SR_TC_Bit)) {
	}
}

void USART_Transmit_String(USART_TypeDef* usart, const char* s) {
	while (*s != '\0') {
		while (!MCL_READ_BIT(usart->SR, USART_SR_TXE_Bit)) {
		}
	
		usart->DR = (uint32_t)(uint8_t)*s++;
	}

	while (!MCL_READ_BIT(usart->SR, USART_SR_TC_Bit)) {
	}
}

uint8_t USART_ByteAvailable(USART_TypeDef* usart) {
	return MCL_READ_BIT(usart->SR, USART_SR_RXNE_Bit);
}

uint8_t USART_Receive_Char(USART_TypeDef* usart, char* c) {
	if (USART_ByteAvailable(usart)) {
		*c = usart->DR;
		return 0;
	}
	return 1;
}

// uint32_t USART_Receive_Line(USART_TypeDef* usart, char* buffer, uint32_t buffer_size) {
// 	uint32_t len = 0;
// 
// 	if (buffer_size == 0) {
// 		return 0;
// 	}
// 
// 	while (1) {
// 		USART_Receive_Char(usart, c);
// 
// 		if (c == '\r') {
// 			buffer[len] = '\0';
// 			USART_Transmit_String(usart, "\r\n");
// 			return len;
// 		}
// 
// 		if (c == '\b' || c == 0x7F) {
// 			if (len > 0) {
// 				len--;
// 				buffer[len] = '\0';
// 				USART_Transmit_String(usart, "\b \b");
// 			}
// 
// 			continue;
// 		}
// 
// 		if (len < buffer_size - 1) {
// 			buffer[len] = c;
// 			len++;
// 			USART_Transmit_Char(usart, c);
// 		}
// 	}
// }
