#pragma once
#include <stdint.h>

#include "../../core/bit_math.h"

#define TIM1_BASE 0x40010000UL
#define TIM8_BASE 0x40010400UL

#define TIM2_BASE 0x40000000UL
#define TIM3_BASE 0x40000400UL
#define TIM4_BASE 0x40000800UL
#define TIM5_BASE 0x40000C00UL

#define TIM6_BASE 0x40001000UL
#define TIM7_BASE 0x40001400UL

typedef struct {
} TIM_ADV_TypeDef;

typedef struct {
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RESERVED0;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t RESERVED1;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t OR;
} TIM_GP_TypeDef;

typedef struct {

} TIM_BASIC_TypeDef;

#define TIM2 ((TIM_GP_TypeDef*) TIM2_BASE)
#define TIM3 ((TIM_GP_TypeDef*) TIM3_BASE)
#define TIM4 ((TIM_GP_TypeDef*) TIM4_BASE)
#define TIM5 ((TIM_GP_TypeDef*) TIM5_BASE)

typedef enum {
	TIM_Channel_1 = 0,
	TIM_Channel_2 = 1,
	TIM_Channel_3 = 2,
	TIM_Channel_4 = 3,
} TIM_Channel;

typedef enum {
	TIM_CR1_CEN_Bit = 0,
	TIM_CR1_DIR_Bit = 4,
	TIM_CR1_ARPE_Bit = 7,
} TIM_CR1_Bit;

typedef enum {
	TIM_Direction_Upcounter = 0,
	TIM_Direction_Downcounter = 1
} TIM_CountDirection;

typedef enum {
	TIM_DIER_UIE_Bit = 0,
} TIM_DIER_Bit;

typedef enum {
	TIM_SR_UIF_Bit = 0,
} TIM_SR_Bit;

typedef enum {
	TIM_EGR_UG_Bit = 0,
} TIM_EGR_Bit;

typedef enum {
	// Capture/compare selection 
	CCMR1_CC1S_Bit   = 0,   // Bits [1:0]
	CCMR1_CC2S_Bit   = 8,   // Bits [9:8]

	// Channel 1: output-compare mode
	CCMR1_OC1FE_Bit  = 2,   // Bit 2
	CCMR1_OC1PE_Bit  = 3,   // Bit 3
	CCMR1_OC1M_Bit   = 4,   // Bits [6:4]
	CCMR1_OC1CE_Bit  = 7,   // Bit 7
	
	// Channel 2: output-compare mode
	CCMR1_OC2FE_Bit  = 10,  // Bit 10
	CCMR1_OC2PE_Bit  = 11,  // Bit 11
	CCMR1_OC2M_Bit   = 12,  // Bits [14:12]
	CCMR1_OC2CE_Bit  = 15,  // Bit 15
	
	// Channel 1: input-capture mode
	CCMR1_IC1PSC_Bit = 2,   // Bits [3:2]
	CCMR1_IC1F_Bit   = 4,   // Bits [7:4]
	
	// Channel 2: input-capture mode
	CCMR1_IC2PSC_Bit = 10,  // Bits [11:10]
	CCMR1_IC2F_Bit   = 12,  // Bits [15:12]
} CCMR1_Bit;

typedef enum {
	// Capture/compare selection
	CCMR2_CC3S_Bit   = 0,   // Bits [1:0]
	CCMR2_CC4S_Bit   = 8,   // Bits [9:8]

	// Channel 3: output-compare mode
	CCMR2_OC3FE_Bit  = 2,   // Bit 2
	CCMR2_OC3PE_Bit  = 3,   // Bit 3
	CCMR2_OC3M_Bit   = 4,   // Bits [6:4]
	CCMR2_OC3CE_Bit  = 7,   // Bit 7

	// Channel 4: output-compare mode
	CCMR2_OC4FE_Bit  = 10,  // Bit 10
	CCMR2_OC4PE_Bit  = 11,  // Bit 11
	CCMR2_OC4M_Bit   = 12,  // Bits [14:12]
	CCMR2_OC4CE_Bit  = 15,  // Bit 15

	// Channel 3: input-capture mode
	CCMR2_IC3PSC_Bit = 2,   // Bits [3:2]
	CCMR2_IC3F_Bit   = 4,   // Bits [7:4]

	// Channel 4: input-capture mode
	CCMR2_IC4PSC_Bit = 10,  // Bits [11:10
	CCMR2_IC4F_Bit   = 12,  // Bits [15:12]
} CCMR2_Bit;

typedef enum {
	CCMR_CCxS_Output = 0b00,

	CCMR_CC1S_InputTI1 = 0b01,
	CCMR_CC1S_InputTI2 = 0b10,
	CCMR_CC2S_InputTI2 = 0b01,
	CCMR_CC2S_InputTI1 = 0b10,
	CCMR_CC3S_InputTI3 = 0b01,
	CCMR_CC3S_InputTI4 = 0b10,
	CCMR_CC4S_InputTI4 = 0b01,
	CCMR_CC4S_InputTI3 = 0b10,

	CCMR_CCxS_InputTRC = 0b11,
} CCMR_CCxS;

#define CCMR_CCxS_WIDTH 2

typedef enum {
	CCMR_OCxM_Frozen = 0b000,
	CCMR_OCxM_ActiveOnMatch = 0b001,
	CCMR_OCxM_InactiveOnMatch = 0b010,
	CCMR_OCxM_Toggle = 0b011,
	CCMR_OCxM_ForceInactiveLevel = 0b100,
	CCMR_OCxM_ForceActiveLevel = 0b101,
	CCMR_OCxM_PWMMode1= 0b110,
	CCMR_OCxM_PWMMode2= 0b111,
} CCMR_OCxM;

#define CCMR_OCxM_WIDTH 3

typedef enum {
	CCER_CC1E_Bit = 0,
	CCER_CC1P_Bit = 1,
	CCER_CC2E_Bit = 4,
	CCER_CC2P_Bit = 5,
	CCER_CC3E_Bit = 8,
	CCER_CC3P_Bit = 9,
	CCER_CC4E_Bit = 12,
	CCER_CC4P_Bit = 13,
} CCER_Bit;

typedef enum {
	TIM_OutputPolarity_ActiveHigh = 0,
	TIM_OutputPolarity_ActiveLow = 1,
} TIM_OutputPolarity;

typedef struct {
	TIM_GP_TypeDef* tim;
	uint32_t prescaler;
	uint32_t auto_reload;
	TIM_CountDirection direction;
	uint8_t auto_reload_preload;
} TIM_Config;

typedef void (*TIM_Callback)(void);

void TIM_Init(TIM_Config* config);
void TIM_Start(TIM_GP_TypeDef* tim);
void TIM_Stop(TIM_GP_TypeDef* tim);
void TIM_Reset(TIM_GP_TypeDef* tim);

void TIM_AutoReloadPreload_Enable(TIM_GP_TypeDef* tim);
void TIM_AutoReloadPreload_Disable(TIM_GP_TypeDef* tim);
void TIM_UpdateInterrupt_Enable(TIM_GP_TypeDef* tim);
void TIM_UpdateInterrupt_Disable(TIM_GP_TypeDef* tim);
void TIM_CaptureCompareSelection_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, CCMR_CCxS selection);
void TIM_OutputComparePreload_Enable(TIM_GP_TypeDef* tim, TIM_Channel channel);
void TIM_OutputComparePreload_Disable(TIM_GP_TypeDef* tim, TIM_Channel channel);
void TIM_OutputCompareMode_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, CCMR_OCxM mode);
void TIM_Channel_Enable(TIM_GP_TypeDef* tim, TIM_Channel channel);
void TIM_Channel_Disable(TIM_GP_TypeDef* tim, TIM_Channel channel);
uint8_t TIM_Channel_IsEnabled(TIM_GP_TypeDef* tim, TIM_Channel channel);
void TIM_OutputPolarity_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, TIM_OutputPolarity polarity);
void TIM_Compare_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint32_t compare);
uint32_t TIM_Compare_Get(TIM_GP_TypeDef* tim, TIM_Channel channel);
uint8_t TIM_UpdateFlag_IsSet(TIM_GP_TypeDef* tim);
void TIM_UpdateFlag_Clear(TIM_GP_TypeDef* tim);
void TIM_UpdateCallback_Register(TIM_GP_TypeDef* tim, TIM_Callback callback);

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
