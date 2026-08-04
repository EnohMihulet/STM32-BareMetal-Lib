#pragma once

#include <stdint.h>
#include "../../core/bit_math.h"


#define NVIC_ISER_BASE  0xE000E100UL
#define NVIC_ICER_BASE  0xE000E180UL

#define NVIC_ISER0_BASE 0xE000E100UL
#define NVIC_ISER1_BASE 0xE000E104UL
#define NVIC_ISER2_BASE 0xE000E108UL
#define NVIC_ISER3_BASE 0xE000E10CUL

typedef volatile uint32_t NVIC_ISER_TypeDef[4];
typedef volatile uint32_t NVIC_ICER_TypeDef[4];

#define NVIC_ISER ((NVIC_ISER_TypeDef*) NVIC_ISER_BASE)
#define NVIC_ICER ((NVIC_ICER_TypeDef*) NVIC_ICER_BASE)

typedef enum {
	TIM2_IRQ_NUMBER = 28,
	TIM3_IRQ_NUMBER = 29,
	TIM4_IRQ_NUMBER = 30,
	TIM5_IRQ_NUMBER = 50,
} IRQ_Number;

static inline void NVIC_IRQ_Enable(IRQ_Number irq_number) {
	*NVIC_ISER[irq_number / 32] = 1UL << (irq_number % 32);
}
static inline void NVIC_IRQ_Disable(IRQ_Number irq_number) {
	*NVIC_ICER[irq_number / 32] = 1UL << (irq_number % 32);
}
