#ifndef K_CPU_H
#define K_CPU_H

#include "include/error.h"
#include "include/types.h"

/* Regular & extended funcions. */
#define K_CPUID_FUNCTION_0x0		0x0
#define K_CPUID_FUNCTION_0x1		0x1
#define K_CPUID_FUNCTION_0x80000000	0x80000000
#define K_CPUID_FUNCTION_0x80000008	0x80000008

/* CPUID 0x1 */
/* EAX */
#define K_CPUID_FUNCTION_0x1_BASE_FAMILY(eax)	((eax >> 8) & 0xf)
#define K_CPUID_FUNCTION_0x1_BASE_MODEL(eax)	((eax >> 4) & 0xf)
#define K_CPUID_FUNCTION_0x1_STEPPING(eax)	((eax >> 0) & 0xf)
#define K_CPUID_FUNCTION_0x1_EXT_FAMILY(eax)	((eax >> 20) & 0xff)
#define K_CPUID_FUNCTION_0x1_EXT_MODEL(eax)	((eax >> 16) & 0xf)

/* EDX */
#define K_CPUID_FUNCTION_0x1_TSC	(1 << 4)
#define K_CPUID_FUNCTION_0x1_MSR	(1 << 5)
#define K_CPUID_FUNCTION_0x1_PAE	(1 << 6)
#define K_CPUID_FUNCTION_0x1_APIC	(1 << 9)

struct k_cpu_x86 {
	char vendor[12];

	int family;
	int model;
	int stepping;
};

void k_cpu_get_info(void);
int k_cpu_eflag(k_uint32_t);
void k_cpu_cpuid(k_uint32_t, k_uint32_t *, k_uint32_t *, k_uint32_t *, k_uint32_t *);

#endif

