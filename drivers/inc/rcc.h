#pragma once
#include <stdint.h>

#include "../../core/bit_math.h"

#define RCC_BASE	0x40023800UL

typedef struct {
	volatile uint32_t CR;        // 0x00
	volatile uint32_t PLLCFGR;   // 0x04
	volatile uint32_t CFGR;      // 0x08
	volatile uint32_t CIR;       // 0x0C
	volatile uint32_t AHB1RSTR;  // 0x10
	uint32_t RESERVED0[7];
	volatile uint32_t AHB1ENR;   // 0x30
	uint32_t RESERVED1[3];
	volatile uint32_t APB1ENR;   // 0x40
	volatile uint32_t APB2ENR;   // 0x44
} RCC_TypeDef;

#define RCC ((RCC_TypeDef *) RCC_BASE)

typedef enum {
	RCC_AHB1_GPIOAEN_Bit = 0,
	RCC_AHB1_GPIOBEN_Bit = 1,
	RCC_AHB1_GPIOCEN_Bit = 2,
	RCC_AHB1_DMA1EN_Bit  = 21,
	RCC_AHB1_DMA2EN_Bit  = 22
} RCC_AHB1ENR_Bit;

static inline void RCC_AHB1Clock_Enable(RCC_AHB1ENR_Bit bit) {
	MCL_SET_BIT(RCC->AHB1ENR, bit);
	(void)(RCC->AHB1ENR);
}

static inline void RCC_AHB1Clock_Disable(RCC_AHB1ENR_Bit bit) {
	MCL_CLEAR_BIT(RCC->AHB1ENR, bit);
	(void)(RCC->AHB1ENR);
}

static inline void RCC_GPIOClock_Enable(uint8_t port) { RCC_AHB1Clock_Enable((RCC_AHB1ENR_Bit)port); }

static inline void RCC_GPIOAClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOAEN_Bit); }
static inline void RCC_GPIOBClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOBEN_Bit); }
static inline void RCC_GPIOCClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOCEN_Bit); }

static inline void RCC_GPIOAClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOAEN_Bit); }
static inline void RCC_GPIOBClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOBEN_Bit); }
static inline void RCC_GPIOCClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOCEN_Bit); }

static inline void RCC_DMA1Clock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_DMA1EN_Bit); }
static inline void RCC_DMA2Clock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_DMA2EN_Bit); }

static inline void RCC_DMA1Clock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_DMA1EN_Bit); }
static inline void RCC_DMA2Clock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_DMA2EN_Bit); }

typedef enum {
	RCC_APB1_TIM2_Bit   = 0,
	RCC_APB1_TIM3_Bit   = 1,
	RCC_APB1_TIM4_Bit   = 2,
	RCC_APB1_TIM5_Bit   = 3,
	RCC_APB1_USART2_Bit = 17,
	RCC_APB1_USART3_Bit = 18,
} RCC_APB1ENR_Bit;

static inline void RCC_APB1Clock_Enable(RCC_APB1ENR_Bit bit) {
	MCL_SET_BIT(RCC->APB1ENR, bit);
	(void)(RCC->APB1ENR);
}

static inline void RCC_APB1Clock_Disable(RCC_APB1ENR_Bit bit) {
	MCL_CLEAR_BIT(RCC->APB1ENR, bit);
	(void)(RCC->APB1ENR);
}

static inline void RCC_USART2Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_USART2_Bit); }
static inline void RCC_USART3Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_USART3_Bit); }

static inline void RCC_USART2Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_USART2_Bit); }
static inline void RCC_USART3Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_USART3_Bit); }

static inline void RCC_TIM2Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_TIM2_Bit); }
static inline void RCC_TIM3Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_TIM3_Bit); }
static inline void RCC_TIM4Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_TIM4_Bit); }
static inline void RCC_TIM5Clock_Enable(void) { RCC_APB1Clock_Enable(RCC_APB1_TIM5_Bit); }

static inline void RCC_TIM2Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_TIM2_Bit); }
static inline void RCC_TIM3Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_TIM3_Bit); }
static inline void RCC_TIM4Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_TIM4_Bit); }
static inline void RCC_TIM5Clock_Disable(void) { RCC_APB1Clock_Disable(RCC_APB1_TIM5_Bit); }
