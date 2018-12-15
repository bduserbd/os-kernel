#ifndef K_MSR_H
#define K_MSR_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#endif

#define K_MSR_IA32_APIC_BASE		0x1b
#define K_MSR_IA32_APIC_BASE_BSP	(1 << 8)	
#define K_MSR_IA32_APIC_BASE_ENABLE	(1 << 11)

#define K_MSR_IA32_EFER		0xc0000080
#define K_MSR_IA32_EFER_SCE	(1 << 0)
#define K_MSR_IA32_EFER_LME	(1 << 8)
#define K_MSR_IA32_EFER_LMA	(1 << 10)
#define K_MSR_IA32_EFER_NXE	(1 << 11)

#ifndef __ASSEMBLER__

static inline void k_rdmsr(k_uint32_t msr, k_uint32_t *low, k_uint32_t *high)
{
	asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

static inline void k_wrmsr(k_uint32_t msr, k_uint32_t low, k_uint32_t high)
{
	asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

#endif

#endif

