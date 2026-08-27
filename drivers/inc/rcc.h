#pragma once
#include <stdint.h>

#include "../../core/bit_math.h"
#include "tim.h"

#define RCC_BASE	0x40023800UL

#ifndef GPIOA_BASE
#define GPIOA_BASE 0x40020000UL
#endif

#ifndef GPIOB_BASE
#define GPIOB_BASE 0x40020400UL
#endif

#ifndef GPIOC_BASE
#define GPIOC_BASE 0x40020800UL
#endif

typedef struct GPIO_TypeDef GPIO_TypeDef;

#ifndef GPIOA
#define GPIOA ((GPIO_TypeDef*) GPIOA_BASE)
#endif

#ifndef GPIOB
#define GPIOB ((GPIO_TypeDef*) GPIOB_BASE)
#endif

#ifndef GPIOC
#define GPIOC ((GPIO_TypeDef*) GPIOC_BASE)
#endif

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
	MCL_CLEAR_BIT(RCC->AHB1ENR, bit); (void)(RCC->AHB1ENR);
}

static inline void RCC_GPIOAClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOAEN_Bit); }
static inline void RCC_GPIOBClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOBEN_Bit); }
static inline void RCC_GPIOCClock_Enable(void) { RCC_AHB1Clock_Enable(RCC_AHB1_GPIOCEN_Bit); }

static inline void RCC_GPIOAClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOAEN_Bit); }
static inline void RCC_GPIOBClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOBEN_Bit); }
static inline void RCC_GPIOCClock_Disable(void) { RCC_AHB1Clock_Disable(RCC_AHB1_GPIOCEN_Bit); }

static inline void RCC_GPIOClock_Enable(GPIO_TypeDef* gpio) {
	if (gpio == GPIOA) {
		RCC_GPIOAClock_Enable();
	}
	else if (gpio == GPIOB) {
		RCC_GPIOBClock_Enable();
	}
	else if (gpio == GPIOC) {
		RCC_GPIOCClock_Enable();
	}
}

static inline void RCC_GPIOClock_Disable(GPIO_TypeDef* gpio) {
	if (gpio == GPIOA) {
		RCC_GPIOAClock_Disable();
	}
	else if (gpio == GPIOB) {
		RCC_GPIOBClock_Disable();
	}
	else if (gpio == GPIOC) {
		RCC_GPIOCClock_Disable();
	}
}

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

static inline void RCC_TIMClock_Enable(TIM_GP_TypeDef* tim) {
	if (tim == TIM2) {
		RCC_TIM2Clock_Enable();
	}
	else if (tim == TIM3) {
		RCC_TIM3Clock_Enable();
	}
	else if (tim == TIM4) {
		RCC_TIM4Clock_Enable();
	}
	else if (tim == TIM5) {
		RCC_TIM5Clock_Enable();
	}
}

static inline void RCC_TIMClock_Disable(TIM_GP_TypeDef* tim) {
	if (tim == TIM2) {
		RCC_TIM2Clock_Disable();
	}
	else if (tim == TIM3) {
		RCC_TIM3Clock_Disable();
	}
	else if (tim == TIM4) {
		RCC_TIM4Clock_Disable();
	}
	else if (tim == TIM5) {
		RCC_TIM5Clock_Disable();
	}
}

typedef enum {
	RCC_APB2_USART1_Bit = 4,
	RCC_APB2_SYSCFG_Bit = 14,
} RCC_APB2ENR_Bit;

static inline void RCC_APB2Clock_Enable(RCC_APB2ENR_Bit bit) {
	MCL_SET_BIT(RCC->APB2ENR, bit);
	(void)(RCC->APB2ENR);
}

static inline void RCC_APB2Clock_Disable(RCC_APB2ENR_Bit bit) {
	MCL_CLEAR_BIT(RCC->APB2ENR, bit);
	(void)(RCC->APB2ENR);
}

static inline void RCC_SYSCFGClock_Enable(void)  { RCC_APB2Clock_Enable(RCC_APB2_SYSCFG_Bit); }
static inline void RCC_USART1Clock_Enable(void) { RCC_APB2Clock_Enable(RCC_APB2_USART1_Bit); }

static inline void RCC_SYSCFGClock_Disable(void) { RCC_APB2Clock_Disable(RCC_APB2_SYSCFG_Bit); }
static inline void RCC_USART1Clock_Disable(void) { RCC_APB2Clock_Disable(RCC_APB2_USART1_Bit); }
