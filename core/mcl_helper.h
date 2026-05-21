#include <stdint.h>

#define CLOCK_SPEED_HZ 16000000UL

// NOTE: Just approximation 

#define DELAY_LOOP_CYCLES 3UL

#define LOOPS_PER_MS (CLOCK_SPEED_HZ / 1000UL / DELAY_LOOP_CYCLES)

static inline void delay_loop(uint32_t loops)
{
	volatile uint32_t i;
	for (i = 0; i < loops; i++) {
		__asm__ volatile ("nop");
	}
}

static inline void delay_ms(uint32_t ms)
{
	while (ms--) {
		delay_loop(LOOPS_PER_MS);
	}
}

static inline void delay_s(uint32_t s)
{
	while (s--) {
		delay_ms(1000);
	}
}
