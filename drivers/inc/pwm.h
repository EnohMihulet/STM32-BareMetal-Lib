#pragma once

#include <stdint.h>

#include "tim.h"

#define PWM_DUTY_MAX 1000U

typedef struct {
	TIM_GP_TypeDef* tim;
	TIM_Channel channel;
	uint32_t timer_clock_hz;
	uint32_t frequency_hz;
	uint16_t duty_per_mille;
	TIM_OutputPolarity polarity;
} PWM_ChannelConfig;

void PWM_Channel_Init(PWM_ChannelConfig* config);
void PWM_Channel_Start(TIM_GP_TypeDef* tim, TIM_Channel channel);
void PWM_Channel_Stop(TIM_GP_TypeDef* tim, TIM_Channel channel);
uint8_t PWM_Channel_IsStarted(TIM_GP_TypeDef* tim, TIM_Channel channel);
void PWM_Duty_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint16_t duty_per_mille);
uint16_t PWM_Duty_Get(TIM_GP_TypeDef* tim, TIM_Channel channel);
