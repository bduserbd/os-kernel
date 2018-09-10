#include "include/cpu.h"

struct k_cpu_x86 k_cpu;

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

static void k_cpu_get_vendor(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	k_cpu_cpuid(K_CPUID_FUNCTION_0x0, &eax, &ebx, &ecx, &edx);

	*(k_uint32_t *)&cpu->vendor[0] = ebx;
	*(k_uint32_t *)&cpu->vendor[4] = edx;
	*(k_uint32_t *)&cpu->vendor[8] = ecx;
}

static void k_cpu_get_model(struct k_cpu_x86 *cpu)
{
	int base_family;
	k_uint32_t eax, ebx, ecx, edx;

	k_cpu_cpuid(K_CPUID_FUNCTION_0x1, &eax, &ebx, &ecx, &edx);

	cpu->family = base_family = K_CPUID_FUNCTION_0x1_BASE_FAMILY(eax);
	if (cpu->family == 0xf)
		cpu->family += K_CPUID_FUNCTION_0x1_EXT_FAMILY(eax);

	cpu->model = K_CPUID_FUNCTION_0x1_BASE_MODEL(eax);
	if (base_family == 0xf)
		cpu->model |= K_CPUID_FUNCTION_0x1_EXT_MODEL(eax) << 4;

	cpu->stepping = K_CPUID_FUNCTION_0x1_STEPPING(eax);
}

void k_cpu_get_info(void)
{
	k_cpu_get_vendor(&k_cpu);
	k_cpu_get_model(&k_cpu);
}

