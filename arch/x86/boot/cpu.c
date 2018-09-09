#include "include/cpu.h"
#include "include/eflags.h"

int k_cpu_eflag(k_uint32_t flags)
{
	k_uint32_t a, b;

	asm volatile("pushfl\n"
			"pushfl\n"
			"pop %0\n"
			"mov %0,%1\n"
			"xor %2,%1\n"
			"push %1\n"
			"popfl\n"
			"pushfl\n"
			"pop %1\n"
			"popfl"
			: "=&r" (a), "=&r" (b)
			: "ri" (flags));

	return !!((a ^ b) & flags);
}

void k_cpu_cpuid(k_uint32_t function, k_uint32_t *eax, k_uint32_t *ebx,
		k_uint32_t *ecx, k_uint32_t *edx)
{
	asm volatile(".ifnc %%ebx,%3 ; movl  %%ebx,%3 ; .endif  \n\t"
			"cpuid                                     \n\t"
			".ifnc %%ebx,%3 ; xchgl %%ebx,%3 ; .endif  \n\t"
			: "=a" (*eax), "=c" (*ecx), "=d" (*edx), "=b" (*ebx)
			: "a" (function));
}

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
	k_uint32_t max_std_function;
	k_uint32_t eax, ebx, ecx, edx;

	/* EFLAGS. */
	if (!k_cpu_eflag(K_EFLAGS_AC) || !k_cpu_eflag(K_EFLAGS_ID))
		return K_ERROR_FAILURE;

	/* Vendor. */
	k_cpu_cpuid(K_CPUID_FUNCTION_0x0, &max_std_function, (k_uint32_t *)&vendor[0],
			(k_uint32_t *)&vendor[8], (k_uint32_t *)&vendor[4]);

	if (!k_cpu_is_intel(vendor))
		return K_ERROR_FAILURE;

	/* TSC & MSR support. */
	k_cpu_cpuid(K_CPUID_FUNCTION_0x1, &eax, &ebx, &ecx, &edx);

	if ((edx & K_CPUID_FUNCTION_0x1_TSC) == 0 || (edx & K_CPUID_FUNCTION_0x1_MSR) == 0)
		return K_ERROR_NONE;

	return K_ERROR_NONE;
}

