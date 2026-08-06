#include <stddef.h>
#include "../../core/bit_math.h"
#include "../inc/exti.h"
#include "../inc/syscfg.h"

static EXTI_Callback exti_callbacks[EXTI_GPIO_LINE_COUNT];

static uint8_t EXTI_Port_Get(GPIO_TypeDef* port, SYSCFG_EXTIPort* exti_port) {
	if (port == GPIOA) {
		*exti_port = SYSCFG_EXTIx_PortA;
		return 1;
	}

	if (port == GPIOB) {
		*exti_port = SYSCFG_EXTIx_PortB;
		return 1;
	}

	if (port == GPIOC) {
		*exti_port = SYSCFG_EXTIx_PortC;
		return 1;
	}

	return 0;
}

static void EXTI_Line_Handle(EXTI_Line line) {
	if (line >= EXTI_GPIO_LINE_COUNT || !EXTI_IsPending(line)) return;

	(void)EXTI_Pending_Clear(line);

	if (exti_callbacks[line] != NULL) {
		exti_callbacks[line]();
	}
}

EXTI_Result EXTI_Interrupt_Enable(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	MCL_SET_BIT(EXTI->IMR, line);
	return EXTI_Result_OK;
}

EXTI_Result EXTI_Interrupt_Disable(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	MCL_CLEAR_BIT(EXTI->IMR, line);
	return EXTI_Result_OK;
}

EXTI_Result EXTI_EventRequest_Mask(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	MCL_CLEAR_BIT(EXTI->EMR, line);
	return EXTI_Result_OK;
}

EXTI_Result EXTI_EventRequest_Unmask(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	MCL_SET_BIT(EXTI->EMR, line);
	return EXTI_Result_OK;
}

EXTI_Result EXTI_Trigger_Enable(EXTI_Line line, EXTI_Trigger trigger) {
	if (line >= EXTI_LINE_COUNT || line == EXTI_Line_19) return EXTI_Result_InvalidLine;
	if (trigger > EXTI_Trigger_Both) return EXTI_Result_InvalidTrigger;

	MCL_CLEAR_BIT(EXTI->RTSR, line);
	MCL_CLEAR_BIT(EXTI->FTSR, line);

		switch (trigger) {
		case EXTI_Trigger_Rising: MCL_SET_BIT(EXTI->RTSR, line); break;
		case EXTI_Trigger_Falling: MCL_SET_BIT(EXTI->FTSR, line); break;
		case EXTI_Trigger_Both: MCL_SET_BIT(EXTI->RTSR, line); MCL_SET_BIT(EXTI->FTSR, line); break;
	}

	return EXTI_Result_OK;
}

EXTI_Result EXTI_Trigger_Disable(EXTI_Line line, EXTI_Trigger trigger) {
	if (line >= EXTI_LINE_COUNT || line == EXTI_Line_19) return EXTI_Result_InvalidLine;
	if (trigger > EXTI_Trigger_Both) return EXTI_Result_InvalidTrigger;

	switch (trigger) {
		case EXTI_Trigger_Rising: MCL_CLEAR_BIT(EXTI->RTSR, line); break;
		case EXTI_Trigger_Falling: MCL_CLEAR_BIT(EXTI->FTSR, line); break;
		case EXTI_Trigger_Both: MCL_CLEAR_BIT(EXTI->RTSR, line); MCL_CLEAR_BIT(EXTI->FTSR, line); break;
	}

	return EXTI_Result_OK;
}

uint8_t EXTI_IsPending(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return 0;
	return MCL_READ_BIT(EXTI->PR, line);
}

EXTI_Result EXTI_Pending_Clear(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	EXTI->PR = 1UL << line;
	return EXTI_Result_OK;
}

EXTI_Result EXTI_SoftwareInterrupt_Generate(EXTI_Line line) {
	if (line >= EXTI_LINE_COUNT) return EXTI_Result_InvalidLine;
	MCL_SET_BIT(EXTI->SWIER, line);
	return EXTI_Result_OK;
}

IRQ_Number EXTI_GPIOIRQNumber_Get(EXTI_Line line) {
	if (line >= EXTI_GPIO_LINE_COUNT) return IRQ_NUMBER_NONE;
	if (line <= 4) return (IRQ_Number)(EXTI0_IRQ_NUMBER + line);
	if (line >= 5 && line <= 9) return EXTI9_5_IRQ_NUMBER;
	if (line >= 10 && line <= 15) return EXTI15_10_IRQ_NUMBER;

	return IRQ_NUMBER_NONE;
}

EXTI_Result EXTI_Line_Configure(EXTI_Config* config) {
	if (config == NULL) return EXTI_Result_InvalidConfig;
	if (config->pin >= EXTI_GPIO_LINE_COUNT) return EXTI_Result_InvalidGPIOline;
	if (config->trigger > EXTI_Trigger_Both) return EXTI_Result_InvalidTrigger;

	SYSCFG_EXTIPort exti_port;
	if (!EXTI_Port_Get(config->port, &exti_port)) return EXTI_Result_InvalidPort;

	EXTI_Line line = (EXTI_Line)config->pin;

	if (SYSCFG_EXTISource_Set(line, exti_port) != SYSCFG_Result_OK) return EXTI_Result_InvalidLine;
	(void)EXTI_Trigger_Enable(line, config->trigger);
	(void)EXTI_Pending_Clear(line);

	if (config->event_enable) (void)EXTI_EventRequest_Unmask(line);
	else (void)EXTI_EventRequest_Mask(line);

	IRQ_Number irq_number = EXTI_GPIOIRQNumber_Get(line);

	if (config->interrupt_enable) {
		(void)EXTI_Interrupt_Enable(line);
		NVIC_IRQ_Enable(irq_number);
	}
	else {
		(void)EXTI_Interrupt_Disable(line);
	}

	return EXTI_Result_OK;
}

EXTI_Result EXTI_Callback_Register(EXTI_Line line, EXTI_Callback callback) {
	if (line >= EXTI_GPIO_LINE_COUNT) return EXTI_Result_InvalidGPIOline;

	exti_callbacks[line] = callback;
	return EXTI_Result_OK;
}

void EXTI0_IRQHandler(void) {
	EXTI_Line_Handle(EXTI_Line_0);
}

void EXTI1_IRQHandler(void) {
	EXTI_Line_Handle(EXTI_Line_1);
}

void EXTI2_IRQHandler(void) {
	EXTI_Line_Handle(EXTI_Line_2);
}

void EXTI3_IRQHandler(void) {
	EXTI_Line_Handle(EXTI_Line_3);
}

void EXTI4_IRQHandler(void) {
	EXTI_Line_Handle(EXTI_Line_4);
}

void EXTI9_5_IRQHandler(void) {
	for (EXTI_Line line = EXTI_Line_5; line <= EXTI_Line_9; line++) {
		EXTI_Line_Handle(line);
	}
}

void EXTI15_10_IRQHandler(void) {
	for (EXTI_Line line = EXTI_Line_10; line <= EXTI_Line_15; line++) {
		EXTI_Line_Handle(line);
	}
}
