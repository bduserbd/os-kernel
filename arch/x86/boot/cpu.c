#include "include/cpu.h"
#include "include/eflags.h"

static int k_cpu_is_intel(char vendor[12])
{
	return vendor[0] == 'G' && vendor[1] == 'e' && vendor[2] == 'n' &&
		vendor[3] == 'u' && vendor[4] == 'i' && vendor[5] == 'n' &&
		vendor[6] == 'e' && vendor[7] == 'I' && vendor[8] == 'n' &&
		vendor[9] == 't' && vendor[10] == 'e' && vendor[11] == 'l';
}

k_error_t k_cpu_valid(void)
{
	char vendor[12];
	k_uint32_t max_function;
	k_uint32_t eax, ebx, ecx, edx;

	/* EFLAGS. */
	if (!k_cpu_eflag(K_EFLAGS_AC) || !k_cpu_eflag(K_EFLAGS_ID))
		return K_ERROR_FAILURE;

	/* Vendor. */
	k_cpu_cpuid(0x00000000, &max_function, (k_uint32_t *)&vendor[0],
			(k_uint32_t *)&vendor[8], (k_uint32_t *)&vendor[4]);

	if (!k_cpu_is_intel(vendor))
		return K_ERROR_FAILURE;

	/* Required features. */
	if (max_function >= 0x00000001) {
		k_cpu_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		if ((edx & K_CPUID_TSC) == 0 || (edx & K_CPUID_MSR) == 0 ||
				(edx & K_CPUID_APIC) == 0)
			return K_ERROR_FAILURE;
	} else
		return K_ERROR_FAILURE;

	return K_ERROR_NONE;
}

