#pragma once

#include <stdint.h>

#define SYSCFG_BASE 0x40013800UL

typedef struct {
	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t EXTICR1;
	volatile uint32_t EXTICR2;
	volatile uint32_t EXTICR3;
	volatile uint32_t EXTICR4;
	uint32_t RESERVED0[2];
	volatile uint32_t CMPCR;
	uint32_t RESERVED1[2];
	volatile uint32_t CFGR;
} SYSCFG_TypeDef;

#define SYSCFG ((SYSCFG_TypeDef*)(SYSCFG_BASE))

typedef enum {
	SYSCFG_EXTIx_PortA = 0b0000,
	SYSCFG_EXTIx_PortB = 0b0001,
	SYSCFG_EXTIx_PortC = 0b0010,
	SYSCFG_EXTIx_PortD = 0b0011,
	SYSCFG_EXTIx_PortE = 0b0100,
	SYSCFG_EXTIx_PortF = 0b0101,
	SYSCFG_EXTIx_PortG = 0b0110,
	SYSCFG_EXTIx_PortH = 0b0111, // Only for EXTICR1
} SYSCFG_EXTIPort;

typedef enum {
	SYSCFG_Result_OK,
	SYSCFG_Result_InvalidLine,
	SYSCFG_Result_InvalidPort,
} SYSCFG_Result;

#define SYSCFG_EXTIPORT_WIDTH 4

SYSCFG_Result SYSCFG_EXTISource_Set(uint8_t line, SYSCFG_EXTIPort port);
