#include "../inc/gpio.h"

static uint8_t GPIO_Port_IsValid(GPIO_TypeDef* port) {
	return port == GPIOA || port == GPIOB || port == GPIOC;
}

static uint8_t GPIO_Pin_IsValid(GPIO_Pin pin) {
	return (uint32_t)pin <= (uint32_t)GPIO_Pin_15;
}

void GPIO_Init(GPIO_Config* config) {
	uint32_t shift;
	if (config == 0 || !GPIO_Port_IsValid(config->port) || !GPIO_Pin_IsValid(config->pin)) return;

	MCL_WRITE_FIELD(config->port->OTYPER, config->pin, 1, config->output_type);

	shift = config->pin * 2;

	MCL_WRITE_FIELD(config->port->MODER, shift, 2, config->mode);
	MCL_WRITE_FIELD(config->port->OSPEEDR, shift, 2, config->speed);
	MCL_WRITE_FIELD(config->port->PUPDR, shift, 2, config->pull);

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

		MCL_WRITE_FIELD(*afr, shift, 4, config->alternate_function);
	}
}

void GPIO_WritePin(GPIO_TypeDef* port, GPIO_Pin pin, uint8_t value) {
	if (!GPIO_Port_IsValid(port) || !GPIO_Pin_IsValid(pin)) return;

	if (value) GPIO_SetPin(port, pin);
	else GPIO_ClearPin(port, pin);
}

void GPIO_SetPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	if (!GPIO_Port_IsValid(port) || !GPIO_Pin_IsValid(pin)) return;

	port->BSRR = (1U << pin);
}

void GPIO_ClearPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	if (!GPIO_Port_IsValid(port) || !GPIO_Pin_IsValid(pin)) return;

	port->BSRR = (1U << (pin + 16));
}

void GPIO_TogglePin(GPIO_TypeDef* port, GPIO_Pin pin) {
	if (!GPIO_Port_IsValid(port) || !GPIO_Pin_IsValid(pin)) return;

	MCL_TOGGLE_BIT(port->ODR, pin);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef* port, GPIO_Pin pin) {
	if (!GPIO_Port_IsValid(port) || !GPIO_Pin_IsValid(pin)) return 0;

	return MCL_READ_BIT(port->IDR, pin) ? 1 : 0;
}
