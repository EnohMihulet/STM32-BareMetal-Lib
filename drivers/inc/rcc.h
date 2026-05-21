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
