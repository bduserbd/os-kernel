#ifndef K_CPU_H
#define K_CPU_H

#include "include/error.h"
#include "include/types.h"

/* CPUID 0x00000001 */
/* EAX */
#define K_CPUID_BASE_FAMILY(eax)	((eax >> 8) & 0xf)
#define K_CPUID_BASE_MODEL(eax)		((eax >> 4) & 0xf)
#define K_CPUID_STEPPING(eax)		((eax >> 0) & 0xf)
#define K_CPUID_EXT_FAMILY(eax)		((eax >> 20) & 0xff)
#define K_CPUID_EXT_MODEL(eax)		((eax >> 16) & 0xf)

/* EBX */
#define K_CPUID_INITIAL_APIC_ID(ebx)	((ebx >> 24) & 0xff)

/* EDX */
#define K_CPUID_TSC	(1 << 4)
#define K_CPUID_MSR	(1 << 5)
#define K_CPUID_PAE	(1 << 6)
#define K_CPUID_APIC	(1 << 9)

struct k_cpu_x86 {
	char vendor[12];

	k_uint32_t max_function;

	int family;
	int model;
	int stepping;

	k_uint8_t initial_apic_id;
};

void k_cpu_get_info(void);
int k_cpu_eflag(k_uint32_t);
void k_cpu_cpuid(k_uint32_t, k_uint32_t *, k_uint32_t *, k_uint32_t *, k_uint32_t *);

#endif

