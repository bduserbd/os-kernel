#ifndef K_CPU_H
#define K_CPU_H

#include "include/error.h"
#include "include/types.h"

#define K_CPUID_FUNCTION_0x0		0x0
#define K_CPUID_FUNCTION_0x1		0x1
#define K_CPUID_FUNCTION_0x80000000	0x80000000
#define K_CPUID_FUNCTION_0x80000008	0x80000008

#define K_CPUID_FUNCTION_0x1_TSC	(1 << 4)
#define K_CPUID_FUNCTION_0x1_MSR	(1 << 5)


#endif

