#include "shell.h"
#include "../app/board_button.h"
#include "../app/board_led.h"
#include "../drivers/inc/pwm.h"

#define SHELL_PWM_TIM TIM2
#define SHELL_PWM_CHANNEL TIM_Channel_1

static uint8_t SHELL_CommandStringEquals(const char* a, const char* b) {
	while (*a != '\0' && *b != '\0') {
		if (*a != *b) {
			return 0;
		}

		a++;
		b++;
	}

	return *a == *b;
}

static SHELL_Result SHELL_ParseUnsigned(const char* s, uint32_t* value) {
	uint32_t result = 0;

	if (*s == '\0') {
		return SHELL_RESULT_EMPTY;
	}

	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return SHELL_RESULT_BAD_NUMBER;
		}

		result = (result * 10U) + (uint32_t)(*s - '0');
		s++;
	}

	*value = result;
	return SHELL_RESULT_OK;
}

static void SHELL_WriteUnsigned(uint32_t value) {
	char buf[11];
	uint32_t i = sizeof(buf);

	buf[--i] = '\0';

	if (value == 0) {
		SHELL_Write("0");
		return;
	}

	while (value > 0 && i > 0) {
		buf[--i] = (char)('0' + (value % 10U));
		value /= 10U;
	}

	SHELL_Write(&buf[i]);
}

SHELL_Result SHELL_CommandHelp(int argc, const char* argv[]) {
	if (argc == 0) {
		(void)argv;
	
		SHELL_PrintCommandList();
		return SHELL_RESULT_OK;
	}

	const SHELL_Command* command = SHELL_LookupCommand(argv[0]);
	if (command == 0) {
		return SHELL_RESULT_UNKNOWN_COMMAND;
	}

	SHELL_PrintCommand(command);
	return SHELL_RESULT_OK;
}

SHELL_Result SHELL_CommandLed(int argc, const char* argv[]) {
	(void)argc;

	if (SHELL_CommandStringEquals(argv[0], "on")) {
		LED_On();
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "off")) {
		LED_Off();
		return SHELL_RESULT_OK;
	}

	return SHELL_RESULT_BAD_ARGUMENT;
}

SHELL_Result SHELL_CommandButton(int argc, const char* argv[]) {
	(void)argc;
	(void)argv;

	if (Board_Button_IsPressed()) {
		SHELL_Write("Button: pressed\r\n");
	}
	else {
		SHELL_Write("Button: released\r\n");
	}

	return SHELL_RESULT_OK;
}

SHELL_Result SHELL_CommandPwm(int argc, const char* argv[]) {
	if (SHELL_CommandStringEquals(argv[0], "start")) {
		if (argc != 1) return SHELL_RESULT_ARGUMENT_COUNT;

		PWM_Channel_Start(SHELL_PWM_TIM, SHELL_PWM_CHANNEL);
		SHELL_Write("PWM: started\r\n");
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "stop")) {
		if (argc != 1) return SHELL_RESULT_ARGUMENT_COUNT;

		PWM_Channel_Stop(SHELL_PWM_TIM, SHELL_PWM_CHANNEL);
		SHELL_Write("PWM: stopped\r\n");
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "status")) {
		if (argc != 1) return SHELL_RESULT_ARGUMENT_COUNT;

		SHELL_Write("PWM: ");
		SHELL_Write(PWM_Channel_IsStarted(SHELL_PWM_TIM, SHELL_PWM_CHANNEL) ? "started" : "stopped");
		SHELL_Write(", duty=");
		SHELL_WriteUnsigned(PWM_Duty_Get(SHELL_PWM_TIM, SHELL_PWM_CHANNEL));
		SHELL_Write("/1000 (approx ");
		SHELL_WriteUnsigned(PWM_Duty_Get(SHELL_PWM_TIM, SHELL_PWM_CHANNEL) / 10U);
		SHELL_Write("%)\r\n");
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "set")) {
		if (argc != 2) return SHELL_RESULT_ARGUMENT_COUNT;

		uint32_t duty = 0;
		SHELL_Result result = SHELL_ParseUnsigned(argv[1], &duty);
		if (result != SHELL_RESULT_OK) return result;
		if (duty > PWM_DUTY_MAX) return SHELL_RESULT_OUT_OF_RANGE;

		PWM_Duty_Set(SHELL_PWM_TIM, SHELL_PWM_CHANNEL, (uint16_t)duty);
		SHELL_Write("PWM: duty set to ");
		SHELL_WriteUnsigned(duty);
		SHELL_Write("/1000\r\n");
		return SHELL_RESULT_OK;
	}

	if (SHELL_CommandStringEquals(argv[0], "duty")) {
		if (argc != 2) return SHELL_RESULT_ARGUMENT_COUNT;

		uint32_t percent = 0;
		SHELL_Result result = SHELL_ParseUnsigned(argv[1], &percent);
		if (result != SHELL_RESULT_OK) return result;
		if (percent > 100U) return SHELL_RESULT_OUT_OF_RANGE;

		uint32_t duty = percent * 10U;
		PWM_Duty_Set(SHELL_PWM_TIM, SHELL_PWM_CHANNEL, (uint16_t)duty);
		SHELL_Write("PWM: duty set to ");
		SHELL_WriteUnsigned(percent);
		SHELL_Write("%\r\n");
		return SHELL_RESULT_OK;
	}

	return SHELL_RESULT_BAD_ARGUMENT;
}
