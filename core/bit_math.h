#include <stdint.h>

#define MCL_SET_BIT(val, bit)		 ((val) |= (1UL << (bit)))
#define MCL_CLEAR_BIT(val, bit)		 ((val) &= ~(1UL << (bit)))
#define MCL_TOGGLE_BIT(val, bit)	 ((val) ^= (1UL << (bit)))

#define MCL_READ_BIT(val, bit)		 (((val) >> (bit)) & 1UL)
