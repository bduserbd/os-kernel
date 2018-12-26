#ifndef K_TSC_H
#define K_TSC_H

#include "kernel/include/types.h"

static inline k_uint64_t k_rdtsc(void)
{
	k_uint32_t low, high;

	asm volatile("rdtsc" : "=a" (low), "=d" (high));

	return ((k_uint64_t)high << 32) | low;
}

void k_tsc_init(void);
void k_tsc_sleep(unsigned int);

#endif

