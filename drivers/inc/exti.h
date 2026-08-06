#pragma once

#include <stdint.h>
#include "gpio.h"
#include "nvic.h"

#define EXTI_BASE 0x40013C00UL
#define EXTI_LINE_COUNT 23U
#define EXTI_GPIO_LINE_COUNT 16U

typedef struct {
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
} EXTI_TypeDef;

#define EXTI ((EXTI_TypeDef*) EXTI_BASE)

typedef enum {
	EXTI_Line_0 = 0,
	EXTI_Line_1 = 1,
	EXTI_Line_2 = 2,
	EXTI_Line_3 = 3,
	EXTI_Line_4 = 4,
	EXTI_Line_5 = 5,
	EXTI_Line_6 = 6,
	EXTI_Line_7 = 7,
	EXTI_Line_8 = 8,
	EXTI_Line_9 = 9,
	EXTI_Line_10 = 10,
	EXTI_Line_11 = 11,
	EXTI_Line_12 = 12,
	EXTI_Line_13 = 13,
	EXTI_Line_14 = 14,
	EXTI_Line_15 = 15,
	EXTI_Line_16 = 16,
	EXTI_Line_17 = 17,
	EXTI_Line_18 = 18,
	EXTI_Line_19 = 19,
	EXTI_Line_20 = 20,
	EXTI_Line_21 = 21,
	EXTI_Line_22 = 22,
} EXTI_Line;

typedef enum {
	EXTI_Trigger_Rising,
	EXTI_Trigger_Falling,
	EXTI_Trigger_Both,
} EXTI_Trigger;

typedef enum {
	EXTI_Result_OK,
	EXTI_Result_InvalidLine,
	EXTI_Result_InvalidGPIOline,
	EXTI_Result_InvalidConfig,
	EXTI_Result_InvalidPort,
	EXTI_Result_InvalidTrigger,
} EXTI_Result;

typedef struct {
	GPIO_TypeDef* port;
	GPIO_Pin pin;
	EXTI_Trigger trigger;
	uint8_t interrupt_enable;
	uint8_t event_enable;
} EXTI_Config;

EXTI_Result EXTI_Interrupt_Enable(EXTI_Line line);
EXTI_Result EXTI_Interrupt_Disable(EXTI_Line line);

EXTI_Result EXTI_EventRequest_Mask(EXTI_Line line);
EXTI_Result EXTI_EventRequest_Unmask(EXTI_Line line);

EXTI_Result EXTI_Trigger_Enable(EXTI_Line line, EXTI_Trigger trigger);
EXTI_Result EXTI_Trigger_Disable(EXTI_Line line, EXTI_Trigger trigger);

uint8_t EXTI_IsPending(EXTI_Line line);
EXTI_Result EXTI_Pending_Clear(EXTI_Line line);
EXTI_Result EXTI_SoftwareInterrupt_Generate(EXTI_Line line);

IRQ_Number EXTI_GPIOIRQNumber_Get(EXTI_Line line);

EXTI_Result EXTI_Line_Configure(EXTI_Config* config);

typedef void (*EXTI_Callback)(void);

EXTI_Result EXTI_Callback_Register(EXTI_Line line, EXTI_Callback callback);

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
