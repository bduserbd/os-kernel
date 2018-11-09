#ifndef K_MSR_H
#define K_MSR_H

#include "kernel/include/types.h"

#define K_MSR_IA32_APIC_BASE		0x1b
#define K_MSR_IA32_APIC_BASE_BSP	(1 << 8)	
#define K_MSR_IA32_APIC_BASE_ENABLE	(1 << 11)

static inline void k_rdmsr(k_uint32_t msr, k_uint32_t *low, k_uint32_t *high)
{
	asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

static inline void k_wrmsr(k_uint32_t msr, k_uint32_t low, k_uint32_t high)
{
	asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

#endif

