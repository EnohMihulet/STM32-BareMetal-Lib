#include <stdint.h>
#include "../../core/bit_math.h"
#include "../inc/rcc.h"
#include "../inc/syscfg.h"

SYSCFG_Result SYSCFG_EXTISource_Set(uint8_t line, SYSCFG_EXTIPort port) {
	if (line >= 16) return SYSCFG_Result_InvalidLine;
	if (port > SYSCFG_EXTIx_PortH) return SYSCFG_Result_InvalidPort;

	RCC_SYSCFGClock_Enable();

	uint16_t register_index = line / 4;
	uint16_t field_position = (line % 4) * 4;

	switch (register_index) {
		case 0: MCL_WRITE_FIELD(SYSCFG->EXTICR1, field_position, 4, port); break;
		case 1: MCL_WRITE_FIELD(SYSCFG->EXTICR2, field_position, 4, port); break;
		case 2: MCL_WRITE_FIELD(SYSCFG->EXTICR3, field_position, 4, port); break;
		case 3: MCL_WRITE_FIELD(SYSCFG->EXTICR4, field_position, 4, port); break;
		default: return SYSCFG_Result_InvalidLine;
	}

	return SYSCFG_Result_OK;
}
