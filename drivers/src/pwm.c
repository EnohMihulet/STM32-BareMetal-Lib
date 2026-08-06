#include "../inc/pwm.h"

static uint8_t PWM_Channel_IsValid(TIM_Channel channel) {
	return channel <= TIM_Channel_4;
}

static uint32_t PWM_Prescaler_Calculate(uint32_t timer_clock_hz, uint32_t frequency_hz) {
	uint32_t prescaler = timer_clock_hz / (frequency_hz * PWM_DUTY_MAX);
	if (prescaler == 0) prescaler = 1;

	return prescaler;
}

static uint32_t PWM_AutoReload_Calculate(uint32_t timer_clock_hz, uint32_t frequency_hz, uint32_t prescaler) {
	return timer_clock_hz / (frequency_hz * prescaler);
}

static PWM_Result PWM_TimerPeriod_Set(TIM_GP_TypeDef* tim, uint32_t timer_clock_hz, uint32_t frequency_hz) {
	if (tim == 0) return PWM_Result_InvalidTimer;
	if (timer_clock_hz == 0) return PWM_Result_InvalidClock;
	if (frequency_hz == 0) return PWM_Result_InvalidFrequency;

	uint32_t prescaler = PWM_Prescaler_Calculate(timer_clock_hz, frequency_hz);
	uint32_t auto_reload = PWM_AutoReload_Calculate(timer_clock_hz, frequency_hz, prescaler);
	if (auto_reload == 0) return PWM_Result_InvalidFrequency;

	TIM_Config timer_config = {
		.tim = tim,
		.prescaler = prescaler,
		.auto_reload = auto_reload,
		.direction = TIM_Direction_Upcounter,
		.auto_reload_preload = 1,
	};

	TIM_Init(&timer_config);
	return PWM_Result_OK;
}

PWM_Result PWM_Channel_Init(PWM_ChannelConfig* config) {
	if (config == 0) return PWM_Result_InvalidConfig;
	if (config->tim == 0) return PWM_Result_InvalidTimer;
	if (!PWM_Channel_IsValid(config->channel)) return PWM_Result_InvalidChannel;
	if (config->timer_clock_hz == 0) return PWM_Result_InvalidClock;
	if (config->frequency_hz == 0) return PWM_Result_InvalidFrequency;
	if (config->duty_per_mille > PWM_DUTY_MAX) return PWM_Result_InvalidDuty;

	PWM_Result result = PWM_TimerPeriod_Set(config->tim, config->timer_clock_hz, config->frequency_hz);
	if (result != PWM_Result_OK) return result;

	TIM_CaptureCompareSelection_Set(config->tim, config->channel, CCMR_CCxS_Output);
	TIM_OutputCompareMode_Set(config->tim, config->channel, CCMR_OCxM_PWMMode1);
	TIM_OutputComparePreload_Enable(config->tim, config->channel);
	TIM_OutputPolarity_Set(config->tim, config->channel, config->polarity);
	return PWM_Duty_Set(config->tim, config->channel, config->duty_per_mille);
}

PWM_Result PWM_Channel_Start(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	if (tim == 0) return PWM_Result_InvalidTimer;
	if (!PWM_Channel_IsValid(channel)) return PWM_Result_InvalidChannel;

	TIM_Channel_Enable(tim, channel);
	TIM_Start(tim);
	return PWM_Result_OK;
}

PWM_Result PWM_Channel_Stop(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	if (tim == 0) return PWM_Result_InvalidTimer;
	if (!PWM_Channel_IsValid(channel)) return PWM_Result_InvalidChannel;

	TIM_Channel_Disable(tim, channel);
	return PWM_Result_OK;
}

uint8_t PWM_Channel_IsStarted(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	if (tim == 0 || !PWM_Channel_IsValid(channel)) return 0;

	return TIM_Channel_IsEnabled(tim, channel);
}

PWM_Result PWM_Frequency_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint32_t timer_clock_hz, uint32_t frequency_hz) {
	if (tim == 0) return PWM_Result_InvalidTimer;
	if (!PWM_Channel_IsValid(channel)) return PWM_Result_InvalidChannel;
	if (timer_clock_hz == 0) return PWM_Result_InvalidClock;
	if (frequency_hz == 0) return PWM_Result_InvalidFrequency;

	uint16_t duty = PWM_Duty_Get(tim, channel);
	uint8_t was_started = PWM_Channel_IsStarted(tim, channel);

	TIM_Stop(tim);
	PWM_Result result = PWM_TimerPeriod_Set(tim, timer_clock_hz, frequency_hz);
	if (result != PWM_Result_OK) return result;
	result = PWM_Duty_Set(tim, channel, duty);
	if (result != PWM_Result_OK) return result;

	if (was_started) {
		TIM_Start(tim);
	}

	return PWM_Result_OK;
}

uint32_t PWM_Frequency_Get(TIM_GP_TypeDef* tim, uint32_t timer_clock_hz) {
	if (tim == 0 || timer_clock_hz == 0) return 0;

	uint32_t prescaler = tim->PSC + 1;
	uint32_t auto_reload = tim->ARR + 1;

	if (prescaler == 0 || auto_reload == 0) return 0;

	return timer_clock_hz / (prescaler * auto_reload);
}

PWM_Result PWM_Duty_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint16_t duty_per_mille) {
	if (tim == 0) return PWM_Result_InvalidTimer;
	if (!PWM_Channel_IsValid(channel)) return PWM_Result_InvalidChannel;
	if (duty_per_mille > PWM_DUTY_MAX) return PWM_Result_InvalidDuty;

	uint32_t period = tim->ARR + 1;
	uint32_t compare = (period * duty_per_mille) / PWM_DUTY_MAX;
	TIM_Compare_Set(tim, channel, compare);
	return PWM_Result_OK;
}

uint16_t PWM_Duty_Get(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	if (tim == 0 || !PWM_Channel_IsValid(channel)) return 0;

	uint32_t period = tim->ARR + 1;
	if (period == 0) return 0;

	uint32_t compare = TIM_Compare_Get(tim, channel);
	uint32_t duty = (compare * PWM_DUTY_MAX) / period;
	if (duty > PWM_DUTY_MAX) duty = PWM_DUTY_MAX;

	return (uint16_t)duty;
}
