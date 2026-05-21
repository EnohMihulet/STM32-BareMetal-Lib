#include "../drivers/inc/rcc.h"
#include "../drivers/inc/gpio.h"
#include "../core/mcl_helper.h"

int main(void) {

	RCC_GPIOAClock_Enable();

	GPIO_Config led  = {
		.port = GPIOA,
		.pin = GPIO_Pin_5,
		.mode = GPIO_Mode_Output,
		.output_type = GPIO_Output_PushPull,
		.speed = GPIO_Speed_Low,
		.pull = GPIO_Pull_None,
	};

	GPIO_Init(&led);

	while (1) {

		GPIO_TogglePin(led.port, led.pin);
		delay_ms(1000);

	}

	return 0;
}
