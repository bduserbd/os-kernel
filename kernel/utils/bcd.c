#include "kernel/include/types.h"

unsigned long k_bcd_from(unsigned long bcd)
{
	int i;
	k_uint8_t byte;
	unsigned long result;

	result = 0;

	for (i = sizeof(bcd) - 1; i >= 0; i--) {
		result *= 100;

		byte = (bcd >> (i << 3)) & 0xff;
		result += ((byte & 0xf0) >> 1) + ((byte & 0xf0) >> 3) + (byte & 0xf);
	}

	return result;
}

