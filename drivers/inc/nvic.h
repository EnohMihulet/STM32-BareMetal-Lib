#pragma once

#include <stdint.h>
#include "../../core/bit_math.h"


#define NVIC_ISER_BASE  0xE000E100UL
#define NVIC_ICER_BASE  0xE000E180UL

#define NVIC_ISER ((volatile uint32_t*) NVIC_ISER_BASE)
#define NVIC_ICER ((volatile uint32_t*) NVIC_ICER_BASE)

typedef enum {
	IRQ_NUMBER_NONE = 255,
	EXTI0_IRQ_NUMBER = 6,
	EXTI1_IRQ_NUMBER = 7,
	EXTI2_IRQ_NUMBER = 8,
	EXTI3_IRQ_NUMBER = 9,
	EXTI4_IRQ_NUMBER = 10,
	EXTI9_5_IRQ_NUMBER = 23,
	TIM2_IRQ_NUMBER = 28,
	TIM3_IRQ_NUMBER = 29,
	TIM4_IRQ_NUMBER = 30,
	EXTI15_10_IRQ_NUMBER = 40,
	TIM5_IRQ_NUMBER = 50,
} IRQ_Number;

static inline void NVIC_IRQ_Enable(IRQ_Number irq_number) {
	uint32_t irq = (uint32_t)irq_number;
	if (irq == (uint32_t)IRQ_NUMBER_NONE || irq >= 128U) return;
	NVIC_ISER[irq / 32U] = 1UL << (irq % 32U);
}
static inline void NVIC_IRQ_Disable(IRQ_Number irq_number) {
	uint32_t irq = (uint32_t)irq_number;
	if (irq == (uint32_t)IRQ_NUMBER_NONE || irq >= 128U) return;
	NVIC_ICER[irq / 32U] = 1UL << (irq % 32U);
}
