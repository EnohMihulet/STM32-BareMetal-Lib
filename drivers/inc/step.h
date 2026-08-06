#pragma once

#include <stdint.h>

#include "pwm.h"

#define STEP_DUTY_PER_MILLE 500U

typedef struct {
	TIM_GP_TypeDef* tim;
	TIM_Channel channel;
	uint32_t timer_clock_hz;
	uint32_t frequency_hz;
	TIM_OutputPolarity polarity;
} STEP_ChannelConfig;

typedef enum {
	STEP_Result_OK,
	STEP_Result_InvalidConfig,
	STEP_Result_InvalidFrequency,
	STEP_Result_PWMError,
} STEP_Result;

STEP_Result STEP_Channel_Init(STEP_ChannelConfig* config);
STEP_Result STEP_Channel_Start(STEP_ChannelConfig* config);
STEP_Result STEP_Channel_Stop(STEP_ChannelConfig* config);
uint8_t STEP_Channel_IsStarted(STEP_ChannelConfig* config);
STEP_Result STEP_Frequency_Set(STEP_ChannelConfig* config, uint32_t frequency_hz);
uint32_t STEP_Frequency_Get(STEP_ChannelConfig* config);
