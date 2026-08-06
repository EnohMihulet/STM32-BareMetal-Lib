#include "../inc/step.h"

STEP_Result STEP_Channel_Init(STEP_ChannelConfig* config) {
	if (config == 0) return STEP_Result_InvalidConfig;
	if (config->frequency_hz == 0) return STEP_Result_InvalidFrequency;

	PWM_ChannelConfig pwm_config = {
		.tim = config->tim,
		.channel = config->channel,
		.timer_clock_hz = config->timer_clock_hz,
		.frequency_hz = config->frequency_hz,
		.duty_per_mille = STEP_DUTY_PER_MILLE,
		.polarity = config->polarity,
	};

 	return PWM_Channel_Init(&pwm_config) == PWM_Result_OK ? STEP_Result_OK : STEP_Result_PWMError;
}

STEP_Result STEP_Channel_Start(STEP_ChannelConfig* config) {
	if (config == 0) return STEP_Result_InvalidConfig;

	return PWM_Channel_Start(config->tim, config->channel) == PWM_Result_OK ? STEP_Result_OK : STEP_Result_PWMError;
}

STEP_Result STEP_Channel_Stop(STEP_ChannelConfig* config) {
	if (config == 0) return STEP_Result_InvalidConfig;

	PWM_Result result = PWM_Channel_Stop(config->tim, config->channel);
	if (result != PWM_Result_OK) return STEP_Result_PWMError;

	TIM_Stop(config->tim);
	return STEP_Result_OK;
}

uint8_t STEP_Channel_IsStarted(STEP_ChannelConfig* config) {
	if (config == 0) return 0;

	return PWM_Channel_IsStarted(config->tim, config->channel);
}

STEP_Result STEP_Frequency_Set(STEP_ChannelConfig* config, uint32_t frequency_hz) {
	if (config == 0) return STEP_Result_InvalidConfig;
	if (frequency_hz == 0) return STEP_Result_InvalidFrequency;

	if (PWM_Frequency_Set(config->tim, config->channel, config->timer_clock_hz, frequency_hz) != PWM_Result_OK) return STEP_Result_PWMError;
	if (PWM_Duty_Set(config->tim, config->channel, STEP_DUTY_PER_MILLE) != PWM_Result_OK) return STEP_Result_PWMError;
	config->frequency_hz = frequency_hz;
	return STEP_Result_OK;
}

uint32_t STEP_Frequency_Get(STEP_ChannelConfig* config) {
	if (config == 0) return 0;

	return PWM_Frequency_Get(config->tim, config->timer_clock_hz);
}
