#pragma once

#include <stdint.h>

#define MCL_SET_BIT(val, bit)		 ((val) |= (1UL << (bit)))
#define MCL_CLEAR_BIT(val, bit)		 ((val) &= ~(1UL << (bit)))
#define MCL_TOGGLE_BIT(val, bit)	 ((val) ^= (1UL << (bit)))

#define MCL_READ_BIT(val, bit)		 (((val) >> (bit)) & 1UL)

#define MCL_FIELD_MASK(width, pos)	 (((1u << (width)) - 1u) << (pos))

#define MCL_WRITE_FIELD(reg, pos, width, value) \
	do { \
		uint32_t mask = MCL_FIELD_MASK((width), (pos)); \
		(reg) = ((reg) & ~mask) | ((((uint32_t)(value)) << (pos)) & mask); \
	} while (0)
