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

void USART1_Init() {
	RCC_GPIOAClock_Enable();
	RCC_USART1Clock_Enable();

	GPIO_Config usart = {
		.port = GPIOA,
		.mode = GPIO_Mode_Alt,
		.output_type = GPIO_Output_PushPull,
		.speed = GPIO_Speed_High,
		.pull = GPIO_Pull_Up,
		.alternate_function = AF7,
	};

	usart.pin = GPIO_Pin_9;
	GPIO_Init(&usart);

	usart.pin = GPIO_Pin_10;
	GPIO_Init(&usart);

	USART_Disable(USART1);

	USART_Oversampling16_Enable(USART1);
	USART_BaudRate_Set(USART1, 115200UL);
	USART_Transmitter_Enable(USART1);
	USART_Receiver_Enable(USART1);
	USART_WordLength_8Bits(USART1);
	USART_Parity_Disable(USART1);
	USART_Set_StopBits(USART1, USART_One_Stop);

	USART_Enable(USART1);
}

void USART_Transmit_Char_Queued(USART_TypeDef* usart, const char c) {
	while (!MCL_READ_BIT(usart->SR, USART_SR_TXE_Bit)) {
	}

	usart->DR = ((uint32_t)c & 0xFFU);
}

void USART_Transmit(USART_TypeDef* usart, const char* s, uint32_t length) {
	for (uint32_t i = 0; i < length; i++) {
		while (!MCL_READ_BIT(usart->SR, USART_SR_TXE_Bit)) {
		}
	
		usart->DR = (uint32_t)(uint8_t)*s++;
	}

	while (!MCL_READ_BIT(usart->SR, USART_SR_TC_Bit)) {
	}
}

void USART_Transmit_Char(USART_TypeDef* usart, const char c) {
	USART_Transmit_Char_Queued(usart, c);

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
	uint32_t status = usart->SR;
	uint32_t error_mask = (1UL << USART_SR_PE_Bit) |
		(1UL << USART_SR_FE_Bit) |
		(1UL << USART_SR_NE_Bit) |
		(1UL << USART_SR_ORE_Bit);

	if ((status & error_mask) != 0) {
		(void)usart->DR;
		return 1;
	}

	if ((status & (1UL << USART_SR_RXNE_Bit)) != 0) {
		*c = (char)usart->DR;
		return 0;
	}
	return 1;
}

uint32_t USART_Receive_Line(USART_TypeDef* usart, char* buffer, uint32_t buffer_size) {
	uint32_t len = 0;

	if (buffer_size == 0) {
		return 0;
	}

	while (1) {
		char c;
		while (USART_Receive_Char(usart, &c) != 0) {
		}

		if (c == '\r') {
			buffer[len] = '\0';
			return len;
		}

		if (c == '\b' || c == 0x7F) {
			if (len > 0) {
				len--;
				buffer[len] = '\0';
			}

			continue;
		}

		if (len < buffer_size - 1) {
			buffer[len] = c;
			len++;
		}
	}
}
