#include "../inc/gpio.h"

void GPIO_Init(GPIO_Config* config) {
	uint32_t shift;

	config->port->OTYPER &= ~(0b1U << config->pin);
	config->port->OTYPER |=  (((uint32_t)config->output_type & 0b1U) << config->pin);

	shift = config->pin * 2;

	config->port->MODER &= ~(0b11U << shift);
	config->port->MODER |=  (((uint32_t)config->mode & 0b11U) << shift);

	config->port->OSPEEDR &= ~(0b11U << shift);
	config->port->OSPEEDR |=  (((uint32_t)config->speed & 0b11U) << shift);

	config->port->PUPDR &= ~(0b11U << shift);
	config->port->PUPDR |=  (((uint32_t)config->pull & 0b11U) << shift);

	if (config->mode == GPIO_Mode_Alt) {
		volatile uint32_t* afr;

		if (config->pin < 8) {
			afr = &config->port->AFRL;
			shift = config->pin * 4;
		}
		else {
			afr = &config->port->AFRH;
			shift = (config->pin - 8) * 4;
		}

		*afr &= ~(0xFU << shift);
		*afr |=  (((uint32_t)config->alternate_function & 0xFU) << shift);
	}
}

void GPIO_WritePin(GPIO_TypeDef* port, GPIO_Pin pin, uint8_t value) {
	if (value) GPIO_SetPin(port, pin);
	else GPIO_ClearPin(port, pin);
}

void GPIO_SetPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	port->BSRR = (1U << pin);
}

void GPIO_ClearPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	port->BSRR = (1U << (pin + 16));
}

void GPIO_TogglePin(GPIO_TypeDef* port, GPIO_Pin pin) {
	port->ODR ^= (1U << pin);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	return (port->IDR & (1U << pin)) ? 1 : 0;
}
