#include "../inc/gpio.h"

void GPIO_Init(GPIO_Config* config) {
	config->port->MODER &= ~(config->mode << (config->pin * 2));
	config->port->MODER |=  (config->mode << (config->pin * 2));

	config->port->OTYPER &= ~(config->output_type << config->pin);
	config->port->OTYPER |=  (config->output_type << config->pin);

	config->port->OSPEEDR &= ~(config->speed << (config->pin * 2));
	config->port->OSPEEDR |=  (config->speed << (config->pin * 2));

	config->port->PUPDR &= ~(config->pull << (config->pin * 2));
	config->port->PUPDR |=  (config->pull << (config->pin * 2));
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
