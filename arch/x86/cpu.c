#include "include/cpu.h"
#include "kernel/include/video/print.h"

struct k_cpu_x86 k_boot_cpu;

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
	k_cpu_cpuid(0x00000000, &cpu->max_function, (k_uint32_t *)&cpu->vendor[0],
		(k_uint32_t *)&cpu->vendor[4], (k_uint32_t *)&cpu->vendor[8]);
}

static void k_cpu_get_model(struct k_cpu_x86 *cpu)
{
	int base_family;
	k_uint32_t eax, ebx, ecx, edx;

	k_cpu_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

	cpu->family = base_family = K_CPUID_BASE_FAMILY(eax);
	if (cpu->family == 0xf)
		cpu->family += K_CPUID_EXT_FAMILY(eax);

	cpu->model = K_CPUID_BASE_MODEL(eax);
	if (base_family == 0xf)
		cpu->model |= K_CPUID_EXT_MODEL(eax) << 4;

	cpu->stepping = K_CPUID_STEPPING(eax);
}

void k_cpu_get_id(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	k_cpu_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

	cpu->initial_apic_id = K_CPUID_INITIAL_APIC_ID(ebx);
}

void k_cpu_get_info(void)
{
	k_cpu_get_vendor(&k_boot_cpu);
	k_cpu_get_model(&k_boot_cpu);
	k_cpu_get_id(&k_boot_cpu);

#if 0
	k_puts("Family: "); k_puthex(k_boot_cpu.family); k_puts("\n");
	k_puts("Model: "); k_puthex(k_boot_cpu.model); k_puts("\n");
	k_puts("Family: "); k_puthex(k_boot_cpu.stepping); k_puts("\n");
#endif
}

