#include "../inc/pwm.h"

static uint32_t PWM_AutoReload_Calculate(uint32_t timer_clock_hz, uint32_t frequency_hz, uint32_t prescaler) {
	return timer_clock_hz / (frequency_hz * prescaler);
}

void PWM_Channel_Init(PWM_ChannelConfig* config) {
	if (config->timer_clock_hz == 0 || config->frequency_hz == 0) return;

	uint32_t prescaler = config->timer_clock_hz / (config->frequency_hz * PWM_DUTY_MAX);
	if (prescaler == 0) prescaler = 1;

	uint32_t auto_reload = PWM_AutoReload_Calculate(config->timer_clock_hz, config->frequency_hz, prescaler);
	if (auto_reload == 0) return;

	TIM_Config timer_config = {
		.tim = config->tim,
		.prescaler = prescaler,
		.auto_reload = auto_reload,
		.direction = TIM_Direction_Upcounter,
		.auto_reload_preload = 1,
	};

	TIM_Init(&timer_config);
	TIM_CaptureCompareSelection_Set(config->tim, config->channel, CCMR_CCxS_Output);
	TIM_OutputCompareMode_Set(config->tim, config->channel, CCMR_OCxM_PWMMode1);
	TIM_OutputComparePreload_Enable(config->tim, config->channel);
	TIM_OutputPolarity_Set(config->tim, config->channel, config->polarity);
	PWM_Duty_Set(config->tim, config->channel, config->duty_per_mille);
}

void PWM_Channel_Start(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	TIM_Channel_Enable(tim, channel);
	TIM_Start(tim);
}

void PWM_Channel_Stop(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	TIM_Channel_Disable(tim, channel);
}

uint8_t PWM_Channel_IsStarted(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	return TIM_Channel_IsEnabled(tim, channel);
}

void PWM_Duty_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint16_t duty_per_mille) {
	if (duty_per_mille > PWM_DUTY_MAX) duty_per_mille = PWM_DUTY_MAX;

	uint32_t period = tim->ARR + 1;
	uint32_t compare = (period * duty_per_mille) / PWM_DUTY_MAX;
	TIM_Compare_Set(tim, channel, compare);
}

uint16_t PWM_Duty_Get(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	uint32_t period = tim->ARR + 1;
	if (period == 0) return 0;

	uint32_t compare = TIM_Compare_Get(tim, channel);
	uint32_t duty = (compare * PWM_DUTY_MAX) / period;
	if (duty > PWM_DUTY_MAX) duty = PWM_DUTY_MAX;

	return (uint16_t)duty;
}
