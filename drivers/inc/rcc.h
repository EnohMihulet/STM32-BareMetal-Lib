#include <stdint.h>

#define RCC_BASE = 0x40023800

#define RCC_AHB1ENR_OFFSET = 0x30

typedef struct {
	uint32_t RESERVED0[4];
	volatile uint32_t AHB1RSTR;
	uint32_t RESERVED1[7];
	volatile uint32_t AHB1ENR;
	uint32_t RESERVED2[3];
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	uint32_t RESERVED3[20];
} RCC;


#define RCC_ENABLE_GPIO_CLOCK(REGISTER)	

