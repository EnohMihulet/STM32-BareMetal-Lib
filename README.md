# STM32 Bare-Metal Lib

Reusable STM32F446RE bare-metal support code and small examples.

## Layout

- `core/`: small core helpers and fixed-width integer definitions.
- `drivers/`: reusable RCC, GPIO, USART, TIM, PWM, STEP, SYSCFG, EXTI, and NVIC drivers.
- `startup/`: startup code and vector table.
- `linker/`: STM32F446RE linker script.
- `shell/`: generic blocking USART shell engine.
- `examples/nucleo_shell/`: Nucleo board LED/button shell example.

## Build Example

```sh
make
```
