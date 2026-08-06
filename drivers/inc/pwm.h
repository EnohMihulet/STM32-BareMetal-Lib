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

typedef enum {
	PWM_Result_OK,
	PWM_Result_InvalidConfig,
	PWM_Result_InvalidTimer,
	PWM_Result_InvalidChannel,
	PWM_Result_InvalidClock,
	PWM_Result_InvalidFrequency,
	PWM_Result_InvalidDuty,
} PWM_Result;

PWM_Result PWM_Channel_Init(PWM_ChannelConfig* config);
PWM_Result PWM_Channel_Start(TIM_GP_TypeDef* tim, TIM_Channel channel);
PWM_Result PWM_Channel_Stop(TIM_GP_TypeDef* tim, TIM_Channel channel);
uint8_t PWM_Channel_IsStarted(TIM_GP_TypeDef* tim, TIM_Channel channel);
PWM_Result PWM_Frequency_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint32_t timer_clock_hz, uint32_t frequency_hz);
uint32_t PWM_Frequency_Get(TIM_GP_TypeDef* tim, uint32_t timer_clock_hz);
PWM_Result PWM_Duty_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint16_t duty_per_mille);
uint16_t PWM_Duty_Get(TIM_GP_TypeDef* tim, TIM_Channel channel);
