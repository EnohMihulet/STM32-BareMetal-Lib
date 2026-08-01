#pragma once
#include <stdint.h>
#include "../../core/mcl_helper.h"
#include "../../core/bit_math.h"

#define USART1_BASE 0x40011000
#define USART2_BASE 0x40004400
#define USART3_BASE 0x40004800
#define USART4_BASE 0x40004C00
#define USART5_BASE 0x40005000
#define USART6_BASE 0x40011400

typedef struct {
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_TypeDef;

#define USART1 ((USART_TypeDef*) USART1_BASE)
#define USART2 ((USART_TypeDef*) USART2_BASE)
#define USART3 ((USART_TypeDef*) USART3_BASE)
#define USART4 ((USART_TypeDef*) USART4_BASE)
#define USART5 ((USART_TypeDef*) USART5_BASE)
#define USART6 ((USART_TypeDef*) USART6_BASE)

typedef enum {
	USART_SR_RXNE_Bit = 5,
	USART_SR_TC_Bit = 6,
	USART_SR_TXE_Bit = 7,
} USART_SR_Bit;

static inline void USART_BaudRate_Set(USART_TypeDef* usart, uint32_t baud) {
	usart->BRR = (CLOCK_SPEED_HZ + (baud / 2U)) / baud;
	(void)(usart->BRR);
}

static inline void USART_BaudRate_Reset(USART_TypeDef* usart) {
	usart->BRR = 0;
	(void)(usart->BRR);
}

typedef enum {
	USART_CR1_RE_Bit     = 2,
	USART_CR1_TE_Bit     = 3,
	USART_CR1_PCE_Bit    = 10,
	USART_CR1_M_Bit      = 12,
	USART_CR1_UE_Bit     = 13,
	USART_CR1_OVER8_Bit  = 15,
} USART_CR1_Bit;

static inline void USART_Receiver_Enable(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_RE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Receiver_Disable(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_RE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Transmitter_Enable(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_TE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Transmitter_Disable(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_TE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Parity_Enable(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_PCE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Parity_Disable(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_PCE_Bit);
	(void)(usart->CR1);
}

static inline void USART_WordLength_9Bits(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_M_Bit);
	(void)(usart->CR1);
}

static inline void USART_WordLength_8Bits(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_M_Bit);
	(void)(usart->CR1);
}

static inline void USART_Enable(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_UE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Disable(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_UE_Bit);
	(void)(usart->CR1);
}

static inline void USART_Oversampling8_Enable(USART_TypeDef *usart)
{
	MCL_SET_BIT(usart->CR1, USART_CR1_OVER8_Bit);
	(void)(usart->CR1);
}

static inline void USART_Oversampling16_Enable(USART_TypeDef *usart)
{
	MCL_CLEAR_BIT(usart->CR1, USART_CR1_OVER8_Bit);
	(void)(usart->CR1);
}


typedef enum {
	USART_One_Stop = 0b00,
	USART_Half_Stop = 0b01,
	USART_Two_Stop = 0b10,
	USART_OneHalf_Stop = 0b11,
} USART_Stop;

#define USART_CR2_STOP_Pos     12u
#define USART_CR2_STOP_Width   2u

static inline void USART_Set_StopBits(USART_TypeDef *usart, USART_Stop stop)
{
    MCL_WRITE_FIELD(usart->CR2, USART_CR2_STOP_Pos, USART_CR2_STOP_Width, stop);
    (void)(usart->CR2);
}

void USART2_Init();

void USART_Transmit_Char(USART_TypeDef* usart, const char c);
void USART_Transmit_String(USART_TypeDef* usart, const char* s);
char USART_Receive_Char(USART_TypeDef* usart);
uint32_t USART_Receive_Line(USART_TypeDef* usart, char* buffer, uint32_t buffer_size);
