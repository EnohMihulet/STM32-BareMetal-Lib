#pragma once
#include "rcc.h"

#ifndef GPIOA_BASE
#define GPIOA_BASE 0x40020000UL
#endif

#ifndef GPIOB_BASE
#define GPIOB_BASE 0x40020400UL
#endif

#ifndef GPIOC_BASE
#define GPIOC_BASE 0x40020800UL
#endif

typedef struct GPIO_TypeDef {
	volatile uint32_t MODER;	// mode register
	volatile uint32_t OTYPER;	// output type register
	volatile uint32_t OSPEEDR;	// output speed register
	volatile uint32_t PUPDR;	// pull-up/pull-down register
	volatile uint32_t IDR;		// input data register
	volatile uint32_t ODR;		// output data register
	volatile uint32_t BSRR;		// bit set/reset register
	volatile uint32_t LCKR;		// configuration lock register
	volatile uint32_t AFRL;		// alternate function low
	volatile uint32_t AFRH;		// alternate function high
} GPIO_TypeDef;
#ifndef GPIOA
#define GPIOA ((GPIO_TypeDef*) GPIOA_BASE)
#endif

#ifndef GPIOB
#define GPIOB ((GPIO_TypeDef*) GPIOB_BASE)
#endif

#ifndef GPIOC
#define GPIOC ((GPIO_TypeDef*) GPIOC_BASE)
#endif

typedef enum {
	GPIO_Mode_Input = 0b00,
	GPIO_Mode_Output= 0b01,
	GPIO_Mode_Alt = 0b10,
	GPIO_Mode_Analog = 0b11,
} GPIO_Mode;

typedef enum {
	GPIO_Output_PushPull = 0,
	GPIO_Output_OpenDrain = 1,
} GPIO_OutputType;

typedef enum {
	GPIO_Speed_Low = 0b00,
	GPIO_Speed_Med = 0b01,
	GPIO_Speed_Fast = 0b10,
	GPIO_Speed_High = 0b11,
} GPIO_Speed;

typedef enum {
	GPIO_Pull_None = 0b00,
	GPIO_Pull_Up = 0b01,
	GPIO_Pull_Down = 0b10,
} GPIO_Pull;

typedef enum {
	AF0 = 0,
	AF1 = 1,
	AF2 = 2,
	AF3 = 3,
	AF4 = 4,
	AF5 = 5,
	AF6 = 6,
	AF7 = 7,
	AF8 = 8,
	AF9 = 9,
	AF10 = 10,
	AF11 = 11,
	AF12 = 12,
	AF13 = 13,
	AF14 = 14,
	AF15 = 15,
} GPIO_AlternateFunction;

typedef enum {
	GPIO_Pin_0 = 0,
	GPIO_Pin_1 = 1,
	GPIO_Pin_2 = 2,
	GPIO_Pin_3 = 3,
	GPIO_Pin_4 = 4,
	GPIO_Pin_5 = 5,
	GPIO_Pin_6 = 6,
	GPIO_Pin_7 = 7,
	GPIO_Pin_8 = 8,
	GPIO_Pin_9 = 9,
	GPIO_Pin_10 = 10,
	GPIO_Pin_11 = 11,
	GPIO_Pin_12 = 12,
	GPIO_Pin_13 = 13,
	GPIO_Pin_14 = 14,
	GPIO_Pin_15 = 15
} GPIO_Pin;

typedef struct {
	GPIO_TypeDef* port;
	GPIO_Pin pin;
	GPIO_Mode mode;
	GPIO_OutputType output_type;
	GPIO_Speed speed;
	GPIO_Pull pull;
	GPIO_AlternateFunction alternate_function;
} GPIO_Config;

void GPIO_Init(GPIO_Config* config);

void GPIO_WritePin(GPIO_TypeDef* port, GPIO_Pin pin, uint8_t value);
void GPIO_SetPin(GPIO_TypeDef* port, GPIO_Pin pin);
void GPIO_ClearPin(GPIO_TypeDef* port, GPIO_Pin pin);
void GPIO_TogglePin(GPIO_TypeDef* port, GPIO_Pin pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef* port, GPIO_Pin pin);

typedef enum {
	GPIOA_Port = 0,
	GPIOB_Port = 1,
	GPIOC_Port = 2,
} GPIO_Port;
