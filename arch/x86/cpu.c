#include "include/cpu.h"
#include "kernel/include/string.h"
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
	asm volatile("cpuid"
		: "=a" (*eax), "=c" (*ecx), "=d" (*edx), "=b" (*ebx)
		: "a" (function), "c" (*ecx)
		: "memory");
}

static void k_cpu_get_vendor(struct k_cpu_x86 *cpu)
{
	k_cpu_cpuid(0x00000000, &cpu->max_function, (k_uint32_t *)&cpu->vendor[0],
		(k_uint32_t *)&cpu->vendor[8], (k_uint32_t *)&cpu->vendor[4]);
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

void k_cpu_get_max_extended_function(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	k_cpu_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);

	cpu->max_extended_function = eax;
}

void k_cpu_get_processor_name(struct k_cpu_x86 *cpu)
{
	k_uint32_t i;

	if (cpu->max_extended_function >= 0x80000004)
		for (i = 0; i < 3; i++)
			k_cpu_cpuid(0x80000002 + i, (k_uint32_t *)&cpu->processor_name[i << 4],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 4],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 8],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 12]);
}

void k_cpu_get_cache_info(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	if (cpu->max_function >= 0x00000004) {
		k_uint32_t count = 0;

		while (1) {
			ecx = count;
			k_cpu_cpuid(0x00000004, &eax, &ebx, &ecx, &edx);
			if (K_CPUID_CACHE_TYPE(eax) == K_CPUID_CACHE_TYPE_NULL)
				break;

			k_uint32_t level = K_CPUID_CACHE_LEVEL(eax);
			if (level <= 3) {
				cpu->cache[level].line_size = K_CPUID_CACHE_LINE_SIZE(ebx);
				cpu->cache[level].size += K_CPUID_CACHE_SETS(ecx) *
							cpu->cache[level].line_size *
							K_CPUID_CACHE_PARTITIONS(ebx) *
							K_CPUID_CACHE_WAYS(ebx);
			}

			count++;
		}
	}
}

void k_cpu_print_info(struct k_cpu_x86 *cpu)
{
	k_uint32_t i;

	k_printf("CPU vendor: %s\n", cpu->vendor);

	k_printf("Family: %d Model: %d Stepping: %d\n", cpu->family, cpu->model, cpu->stepping);

	k_printf("Max standard & extendend functions - %x, %x\n", cpu->max_function,
			cpu->max_extended_function);

	k_printf("Processor name: %s\n", cpu->processor_name);

	for (i = 0; i < 3; i++) {
		if (!cpu->cache[i].line_size || !cpu->cache[i].size)
			continue;

		k_printf("Cache L%d - Line size: %d Total size (bytes): %x\n", i,
			cpu->cache[i].line_size, cpu->cache[i].size);
	}
}

void k_cpu_get_info(void)
{
	k_memset(&k_boot_cpu, 0, sizeof(struct k_cpu_x86));

	k_cpu_get_vendor(&k_boot_cpu);
	k_cpu_get_model(&k_boot_cpu);
	k_cpu_get_id(&k_boot_cpu);

	k_cpu_get_max_extended_function(&k_boot_cpu);
	k_cpu_get_processor_name(&k_boot_cpu);

	k_cpu_get_cache_info(&k_boot_cpu);

	k_cpu_print_info(&k_boot_cpu);
}

