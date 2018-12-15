#include "include/cpu.h"
#include "include/eflags.h"

bool k_cpu_eflag(unsigned long flags)
{
	k_uint32_t a, b;

	asm volatile("pushfl		\n"
			"pushfl		\n"
			"pop	%0	\n"
			"mov	%0,%1	\n"
			"xor	%2,%1	\n"
			"push	%1	\n"
			"popfl		\n"
			"pushfl		\n"
			"pop	%1	\n"
			"popfl"
			: "=&r" (a), "=&r" (b)
			: "ri" (flags));

	return !!((a ^ b) & flags);
}

static int k_cpu_is_intel(char vendor[12])
{
	return vendor[0] == 'G' && vendor[1] == 'e' && vendor[2] == 'n' &&
		vendor[3] == 'u' && vendor[4] == 'i' && vendor[5] == 'n' &&
		vendor[6] == 'e' && vendor[7] == 'I' && vendor[8] == 'n' &&
		vendor[9] == 't' && vendor[10] == 'e' && vendor[11] == 'l';
}

k_error_t k_cpu_is_valid(void)
{
	char vendor[12];
	unsigned long max_function;
	unsigned long eax, ebx, ecx, edx;

	if (!k_cpu_eflag(K_EFLAGS_AC) || !k_cpu_eflag(K_EFLAGS_ID))
		return K_ERROR_FAILURE;

	k_cpuid(0x00000000, &max_function, (unsigned long *)&vendor[0],
			(unsigned long *)&vendor[8], (unsigned long *)&vendor[4]);

	if (!k_cpu_is_intel(vendor))
		return K_ERROR_FAILURE;

	if (max_function >= 0x00000001) {
		k_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		if (!(edx & K_CPUID_TSC) || !(edx & K_CPUID_MSR) ||
				!(edx & K_CPUID_APIC) || !(edx & K_CPUID_CMOV))
			return K_ERROR_FAILURE;
	} else
		return K_ERROR_FAILURE;

	return K_ERROR_NONE;
}

