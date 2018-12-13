#include "include/cpu.h"
#include "include/eflags.h"

bool k_cpu_eflag(unsigned long flags)
{
        k_uint64_t a, b;

        asm volatile("pushfq		\n"
                        "pushfq		\n"
                        "popq	%0	\n"
                        "mov	%0,%1	\n"
                        "xorq	%2,%1	\n"
                        "push	%1	\n"
                        "popfq		\n"
                        "pushfq		\n"
                        "popq %1	\n"
                        "popfq"
                        : "=&r" (a), "=&r" (b)
                        : "ri" (flags));

        return !!((a ^ b) & flags);
}

static int k_cpu_is_amd(char vendor[12])
{
	return vendor[0] == 'A' && vendor[1] == 'u' && vendor[2] == 't' &&
		vendor[3] == 'h' && vendor[4] == 'e' && vendor[5] == 'n' &&
		vendor[6] == 't' && vendor[7] == 'i' && vendor[8] == 'c' &&
		vendor[9] == 'A' && vendor[10] == 'M' && vendor[11] == 'D';
}

k_error_t k_cpu_is_valid(void)
{
	char vendor[12];
	k_uint32_t max_function;
	k_uint32_t eax, ebx, ecx, edx;

	if (!k_cpu_eflag(K_EFLAGS_AC) || !k_cpu_eflag(K_EFLAGS_ID))
		return K_ERROR_FAILURE;

	k_cpuid(0x00000000, &max_function, (k_uint32_t *)&vendor[0],
			(k_uint32_t *)&vendor[8], (k_uint32_t *)&vendor[4]);

	if (!k_cpu_is_amd(vendor))
		return K_ERROR_FAILURE;

	if (max_function >= 0x00000001) {
		k_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		if (!(edx & K_CPUID_PSE) || !(edx & K_CPUID_TSC) || !(edx & K_CPUID_MSR) ||
				!(edx & K_CPUID_PAE) || !(edx & K_CPUID_APIC) ||
				!(edx & K_CPUID_PGE))
			return K_ERROR_FAILURE;

		k_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
		if (eax >= 0x80000001) {
			k_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);

			if (!(edx & K_CPUID_LM))
				return K_ERROR_FAILURE;
		} else
			return K_ERROR_FAILURE;
	} else
		return K_ERROR_FAILURE;

	return K_ERROR_NONE;
}

