#include "../inc/tim.h"

static TIM_Callback tim2_update_callback;
static TIM_Callback tim3_update_callback;
static TIM_Callback tim4_update_callback;
static TIM_Callback tim5_update_callback;

static void TIM_UpdateInterrupt_Handle(TIM_GP_TypeDef* tim, TIM_Callback callback) {
	if (TIM_UpdateFlag_IsSet(tim)) {
		TIM_UpdateFlag_Clear(tim);
		if (callback != 0) {
			callback();
		}
	}
}

void TIM_Init(TIM_Config* config) {
	if (config->prescaler == 0 || config->auto_reload == 0) return;

	TIM_Stop(config->tim);

	if (config->direction == TIM_Direction_Downcounter) {
		MCL_SET_BIT(config->tim->CR1, TIM_CR1_DIR_Bit);
		config->tim->CNT = config->tim->ARR;
	}
	else {
		MCL_CLEAR_BIT(config->tim->CR1, TIM_CR1_DIR_Bit);
		config->tim->CNT = 0;
	}

	if (config->auto_reload_preload == 0) TIM_AutoReloadPreload_Disable(config->tim);
	else TIM_AutoReloadPreload_Enable(config->tim);

	config->tim->PSC = config->prescaler - 1;
	config->tim->ARR = config->auto_reload - 1;

	MCL_SET_BIT(config->tim->EGR, TIM_EGR_UG_Bit);
}

void TIM_Start(TIM_GP_TypeDef* tim) {
	MCL_SET_BIT(tim->CR1, TIM_CR1_CEN_Bit);
	(void)(tim->CR1);
}

void TIM_Stop(TIM_GP_TypeDef* tim) {
	MCL_CLEAR_BIT(tim->CR1, TIM_CR1_CEN_Bit);
	(void)(tim->CR1);
}

void TIM_Reset(TIM_GP_TypeDef* tim) {
	TIM_Stop(tim);

	if (MCL_READ_BIT(tim->CR1, TIM_CR1_DIR_Bit) == TIM_Direction_Upcounter) {
		tim->CNT = 0;
	}
	else {
		tim->CNT = tim->ARR;
	}
	
	MCL_CLEAR_BIT(tim->SR, TIM_SR_UIF_Bit);
}

void TIM_AutoReloadPreload_Enable(TIM_GP_TypeDef* tim) {
	MCL_SET_BIT(tim->CR1, TIM_CR1_ARPE_Bit);
	(void)(tim->CR1);
}

void TIM_AutoReloadPreload_Disable(TIM_GP_TypeDef* tim) {
	MCL_CLEAR_BIT(tim->CR1, TIM_CR1_ARPE_Bit);
	(void)(tim->CR1);
}

void TIM_UpdateInterrupt_Enable(TIM_GP_TypeDef* tim) {
	MCL_SET_BIT(tim->DIER, TIM_DIER_UIE_Bit);
	(void)(tim->DIER);
}

void TIM_UpdateInterrupt_Disable(TIM_GP_TypeDef* tim) {
	MCL_CLEAR_BIT(tim->DIER, TIM_DIER_UIE_Bit);
	(void)(tim->DIER);
}

void TIM_CaptureCompareSelection_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, CCMR_CCxS selection) {
	switch (channel) {
		case TIM_Channel_1: MCL_WRITE_FIELD(tim->CCMR1, CCMR1_CC1S_Bit, CCMR_CCxS_WIDTH, selection); break;
		case TIM_Channel_2: MCL_WRITE_FIELD(tim->CCMR1, CCMR1_CC2S_Bit, CCMR_CCxS_WIDTH, selection); break;
		case TIM_Channel_3: MCL_WRITE_FIELD(tim->CCMR2, CCMR2_CC3S_Bit, CCMR_CCxS_WIDTH, selection); break;
		case TIM_Channel_4: MCL_WRITE_FIELD(tim->CCMR2, CCMR2_CC4S_Bit, CCMR_CCxS_WIDTH, selection); break;
	}
}

void TIM_OutputComparePreload_Enable(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: MCL_SET_BIT(tim->CCMR1, CCMR1_OC1PE_Bit); break;
		case TIM_Channel_2: MCL_SET_BIT(tim->CCMR1, CCMR1_OC2PE_Bit); break;
		case TIM_Channel_3: MCL_SET_BIT(tim->CCMR2, CCMR2_OC3PE_Bit); break;
		case TIM_Channel_4: MCL_SET_BIT(tim->CCMR2, CCMR2_OC4PE_Bit); break;
	}
}

void TIM_OutputComparePreload_Disable(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: MCL_CLEAR_BIT(tim->CCMR1, CCMR1_OC1PE_Bit); break;
		case TIM_Channel_2: MCL_CLEAR_BIT(tim->CCMR1, CCMR1_OC2PE_Bit); break;
		case TIM_Channel_3: MCL_CLEAR_BIT(tim->CCMR2, CCMR2_OC3PE_Bit); break;
		case TIM_Channel_4: MCL_CLEAR_BIT(tim->CCMR2, CCMR2_OC4PE_Bit); break;
	}
}

void TIM_OutputCompareMode_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, CCMR_OCxM mode) {
	switch (channel) {
		case TIM_Channel_1: MCL_WRITE_FIELD(tim->CCMR1, CCMR1_OC1M_Bit, CCMR_OCxM_WIDTH, mode); break;
		case TIM_Channel_2: MCL_WRITE_FIELD(tim->CCMR1, CCMR1_OC2M_Bit, CCMR_OCxM_WIDTH, mode); break;
		case TIM_Channel_3: MCL_WRITE_FIELD(tim->CCMR2, CCMR2_OC3M_Bit, CCMR_OCxM_WIDTH, mode); break;
		case TIM_Channel_4: MCL_WRITE_FIELD(tim->CCMR2, CCMR2_OC4M_Bit, CCMR_OCxM_WIDTH, mode); break;
	}
}

void TIM_Channel_Enable(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: MCL_SET_BIT(tim->CCER, CCER_CC1E_Bit); break;
		case TIM_Channel_2: MCL_SET_BIT(tim->CCER, CCER_CC2E_Bit); break;
		case TIM_Channel_3: MCL_SET_BIT(tim->CCER, CCER_CC3E_Bit); break;
		case TIM_Channel_4: MCL_SET_BIT(tim->CCER, CCER_CC4E_Bit); break;
	}
}

void TIM_Channel_Disable(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: MCL_CLEAR_BIT(tim->CCER, CCER_CC1E_Bit); break;
		case TIM_Channel_2: MCL_CLEAR_BIT(tim->CCER, CCER_CC2E_Bit); break;
		case TIM_Channel_3: MCL_CLEAR_BIT(tim->CCER, CCER_CC3E_Bit); break;
		case TIM_Channel_4: MCL_CLEAR_BIT(tim->CCER, CCER_CC4E_Bit); break;
	}
}

uint8_t TIM_Channel_IsEnabled(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: return MCL_READ_BIT(tim->CCER, CCER_CC1E_Bit);
		case TIM_Channel_2: return MCL_READ_BIT(tim->CCER, CCER_CC2E_Bit);
		case TIM_Channel_3: return MCL_READ_BIT(tim->CCER, CCER_CC3E_Bit);
		case TIM_Channel_4: return MCL_READ_BIT(tim->CCER, CCER_CC4E_Bit);
	}

	return 0;
}

void TIM_OutputPolarity_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, TIM_OutputPolarity polarity) {
	switch (channel) {
		case TIM_Channel_1: MCL_WRITE_FIELD(tim->CCER, CCER_CC1P_Bit, 1, polarity); break;
		case TIM_Channel_2: MCL_WRITE_FIELD(tim->CCER, CCER_CC2P_Bit, 1, polarity); break;
		case TIM_Channel_3: MCL_WRITE_FIELD(tim->CCER, CCER_CC3P_Bit, 1, polarity); break;
		case TIM_Channel_4: MCL_WRITE_FIELD(tim->CCER, CCER_CC4P_Bit, 1, polarity); break;
	}
}

void TIM_Compare_Set(TIM_GP_TypeDef* tim, TIM_Channel channel, uint32_t compare) {
	switch (channel) {
		case TIM_Channel_1: tim->CCR1 = compare; break;
		case TIM_Channel_2: tim->CCR2 = compare; break;
		case TIM_Channel_3: tim->CCR3 = compare; break;
		case TIM_Channel_4: tim->CCR4 = compare; break;
	}
}

uint32_t TIM_Compare_Get(TIM_GP_TypeDef* tim, TIM_Channel channel) {
	switch (channel) {
		case TIM_Channel_1: return tim->CCR1;
		case TIM_Channel_2: return tim->CCR2;
		case TIM_Channel_3: return tim->CCR3;
		case TIM_Channel_4: return tim->CCR4;
	}

	return 0;
}

uint8_t TIM_UpdateFlag_IsSet(TIM_GP_TypeDef* tim) {
	return MCL_READ_BIT(tim->SR, TIM_SR_UIF_Bit);
}

void TIM_UpdateFlag_Clear(TIM_GP_TypeDef* tim) {
	MCL_CLEAR_BIT(tim->SR, TIM_SR_UIF_Bit);
}

void TIM_UpdateCallback_Register(TIM_GP_TypeDef* tim, TIM_Callback callback) {
	if (tim == TIM2) {
		tim2_update_callback = callback;
	}
	else if (tim == TIM3) {
		tim3_update_callback = callback;
	}
	else if (tim == TIM4) {
		tim4_update_callback = callback;
	}
	else if (tim == TIM5) {
		tim5_update_callback = callback;
	}
}

void TIM2_IRQHandler(void) {
	TIM_UpdateInterrupt_Handle(TIM2, tim2_update_callback);
}

void TIM3_IRQHandler(void) {
	TIM_UpdateInterrupt_Handle(TIM3, tim3_update_callback);
}

void TIM4_IRQHandler(void) {
	TIM_UpdateInterrupt_Handle(TIM4, tim4_update_callback);
}

void TIM5_IRQHandler(void) {
	TIM_UpdateInterrupt_Handle(TIM5, tim5_update_callback);
}
