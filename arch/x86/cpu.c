#include "include/cpu.h"
#include "kernel/include/string.h"
#include "kernel/include/video/print.h"

struct k_cpu_x86 k_boot_cpu;

void k_cpuid_count(k_uint32_t function, k_uint32_t *eax, k_uint32_t *ebx,
		k_uint32_t *ecx, k_uint32_t *edx)
{
	asm volatile("cpuid"
		: "=a" (*eax), "=c" (*ecx), "=d" (*edx), "=b" (*ebx)
		: "a" (function), "c" (*ecx)
		: "memory");
}

void k_cpuid(k_uint32_t function, unsigned long *eax, unsigned long *ebx,
		unsigned long *ecx, unsigned long *edx)
{
	asm volatile("cpuid"
		: "=a" (*eax), "=c" (*ecx), "=d" (*edx), "=b" (*ebx)
		: "a" (function)
		: "memory");
}

#if 0
static void k_cpu_get_vendor(struct k_cpu_x86 *cpu)
{
	k_cpuid(0x00000000, &cpu->max_function, (k_uint32_t *)&cpu->vendor[0],
		(k_uint32_t *)&cpu->vendor[8], (k_uint32_t *)&cpu->vendor[4]);
}

static void k_cpu_get_model(struct k_cpu_x86 *cpu)
{
	int base_family;
	k_uint32_t eax, ebx, ecx, edx;

	if (cpu->max_function >= 0x00000001) {
		k_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		cpu->family = base_family = K_CPUID_BASE_FAMILY(eax);
		if (cpu->family == 0xf)
			cpu->family += K_CPUID_EXT_FAMILY(eax);

		cpu->model = K_CPUID_BASE_MODEL(eax);
		if (base_family == 0xf)
			cpu->model |= K_CPUID_EXT_MODEL(eax) << 4;

		cpu->stepping = K_CPUID_STEPPING(eax);
	}
}

void k_cpu_get_id(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	if (cpu->max_function >= 0x00000001) {
		k_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		cpu->initial_apic_id = K_CPUID_INITIAL_APIC_ID(ebx);
	}
}

void k_cpu_get_max_extended_function(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	k_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);

	if (eax <= 0x80000000)
		cpu->max_extended_function = 0;
	else
		cpu->max_extended_function = eax;
}

void k_cpu_get_processor_name(struct k_cpu_x86 *cpu)
{
	k_uint32_t i;

	if (cpu->max_extended_function >= 0x80000004)
		for (i = 0; i < 3; i++)
			k_cpuid(0x80000002 + i, (k_uint32_t *)&cpu->processor_name[i << 4],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 4],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 8],
				(k_uint32_t *)&cpu->processor_name[(i << 4) + 12]);
}

static struct k_cpuid2_descriptor k_cpuid2_encoding_descriptors[0xff] = {
	[0x06] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_INSTRUCTION, K_KB(8), 4, 32),
	[0x08] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_INSTRUCTION, K_KB(16), 4, 32),
	[0x09] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_INSTRUCTION, K_KB(32), 4, 64),
	[0x0a] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(8), 2, 32),
	[0x0c] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(16), 4, 32),
	[0x0d] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(16), 4, 64),
	[0x0e] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(24), 6, 64),
	[0x1d] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(128), 2, 64),
	[0x21] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(256), 8, 64),
	[0x22] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_KB(512), 4, 64),
	[0x23] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(1), 8, 64),
	[0x24] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 16, 64),
	[0x25] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(2), 8, 64),
	[0x29] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(4), 8, 64),
	[0x2c] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(32), 8, 64),
	[0x30] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_INSTRUCTION, K_KB(32), 8, 64),
	[0x41] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(128), 4, 32),
	[0x42] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(256), 4, 32),
	[0x43] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 4, 32),
	[0x44] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 4, 32),
	[0x45] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(2), 4, 32),
	[0x46] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(4), 4, 64),
	[0x47] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(8), 8, 64),
	[0x48] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(3), 12, 64),
	[0x4a] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(6), 12, 64),
	[0x4b] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(8), 16, 64),
	[0x4c] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(12), 12, 64),
	[0x4d] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(16), 16, 64),
	[0x4e] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(6), 24, 64),
	[0x60] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(16), 8, 64),
	[0x66] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(8), 4, 64),
	[0x67] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(16), 4, 64),
	[0x68] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L1_DATA, K_KB(32), 4, 64),
	[0x78] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 4, 64),
	[0x79] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(128), 8, 64),
	[0x7a] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(256), 8, 64),
	[0x7b] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 8, 64),
	[0x7c] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 8, 64),
	[0x7d] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(2), 8, 64),
	[0x7f] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 2, 64),
	[0x80] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 8, 64),
	[0x82] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(256), 8, 32),
	[0x83] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 8, 32),
	[0x84] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 8, 32),
	[0x85] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(2), 8, 32),
	[0x86] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_KB(512), 4, 64),
	[0x87] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L2, K_MB(1), 8, 64),
	[0xd0] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_KB(512), 4, 64),
	[0xd1] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(1), 4, 64),
	[0xd2] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(2), 4, 64),
	[0xd6] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(1), 8, 64),
	[0xd7] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(2), 8, 64),
	[0xd8] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(4), 8, 64),
	[0xdc] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(1) + (K_MB(1) >> 1), 12, 64),
	[0xdd] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(3), 12, 64),
	[0xde] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(6), 12, 64),
	[0xe2] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(2), 16, 64),
	[0xe3] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(4), 16, 64),
	[0xe4] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(8), 16, 64),
	[0xea] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(12), 24, 64),
	[0xeb] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(18), 24, 64),
	[0xec] = K_CPUID2_CACHE_ENTRY(K_CPU_CACHE_TYPE_L3, K_MB(24), 24, 64),
};

void k_cpu_get_cache_info(struct k_cpu_x86 *cpu)
{
	k_uint32_t eax, ebx, ecx, edx;

	if (cpu->max_function >= 0x00000001) {
		k_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		if (edx & K_CPUID_CLFSH)
			cpu->clflush_line_size = K_CPUID_CLFLUSH_LINE_SIZE(ebx);
	}

	if (cpu->max_function >= 0x00000004) {
		k_uint32_t count = 0;

		while (1) {
			ecx = count;
			k_cpuid_count(0x00000004, &eax, &ebx, &ecx, &edx);
			if (K_CPUID_CACHE_TYPE(eax) == K_CPUID_CACHE_TYPE_NULL)
				break;

			int type;
			k_uint32_t level = K_CPUID_CACHE_LEVEL(eax);

			if (level == 1) {
				if (K_CPUID_CACHE_TYPE(eax) == K_CPUID_CACHE_TYPE_DATA)
					type = K_CPU_CACHE_TYPE_L1_DATA;
				else if (K_CPUID_CACHE_TYPE(eax) == K_CPUID_CACHE_TYPE_INSTRUCTION)
					type = K_CPU_CACHE_TYPE_L1_INSTRUCTION;
				else
					goto _out;
			} else if (level == 2)
				type = K_CPU_CACHE_TYPE_L2;
			else if (level == 3)
				type = K_CPU_CACHE_TYPE_L3;
			else
				goto _out;

			cpu->cache[type].line_size = K_CPUID_CACHE_LINE_SIZE(ebx);
			cpu->cache[type].way_associative = K_CPUID_CACHE_WAYS(ebx);
			cpu->cache[type].size = K_CPUID_CACHE_SETS(ecx) *
						cpu->cache[type].line_size *
						K_CPUID_CACHE_PARTITIONS(ebx) *
						cpu->cache[type].way_associative;

_out:
			count++;
		}

		return;
	}

	if (cpu->max_function >= 0x00000002) {
		k_uint32_t i;

		k_cpuid(0x00000002, &eax, &ebx, &ecx, &edx);

#define GET_CACHE_INFO(reg)										\
	if ((reg & (1 << 31)) == 0) {									\
		k_uint32_t times = reg == eax ? 3 : 4;							\
		for (i = 0; i < times; i++) {								\
			struct k_cpuid2_descriptor *entry =						\
				&k_cpuid2_encoding_descriptors[(reg >> (24 - (i * 8))) & 0xff];		\
			if (entry->type == K_CPUID2_TYPE_CACHE) {					\
				cpu->cache[entry->cache.type].size = entry->cache.size;			\
				cpu->cache[entry->cache.type].way_associative = entry->cache.way_associative;\
				cpu->cache[entry->cache.type].line_size = entry->cache.line_size;	\
			}										\
		}											\
	}

		GET_CACHE_INFO(eax);
		GET_CACHE_INFO(ebx);
		GET_CACHE_INFO(ecx);
		GET_CACHE_INFO(edx);
	}
}
#endif

void k_cpu_print_info(struct k_cpu_x86 *cpu)
{
#if 0
	k_uint32_t i;

	k_printf("CPU vendor: %s\n", cpu->vendor);

	k_printf("Family: %d Model: %d Stepping: %d\n", cpu->family, cpu->model, cpu->stepping);

	k_printf("Max standard & extendend functions - %x, %x\n", cpu->max_function,
			cpu->max_extended_function);

	k_printf("Processor name: %s\n", cpu->processor_name);

	for (i = 0; i < K_CPU_CACHE_MAX; i++) {
		if (!cpu->cache[i].line_size || !cpu->cache[i].size)
			continue;

		k_printf("Cache %s - Total size (bytes): %x Way associative: %d  Line size: %d\n",
			i == K_CPU_CACHE_TYPE_L1_DATA ? "L1 (data)" :
			i == K_CPU_CACHE_TYPE_L1_INSTRUCTION ? "L1 (instruction)" :
			i == K_CPU_CACHE_TYPE_L2 ? "L2" :
			i == K_CPU_CACHE_TYPE_L3 ? "L3" : "Unknown",
			cpu->cache[i].size, cpu->cache[i].way_associative, cpu->cache[i].line_size);
	}
#endif
}

unsigned int k_cpu_cache_line_size(void)
{
	return k_boot_cpu.cache[K_CPU_CACHE_TYPE_L1_DATA].line_size;
}

void k_cpu_get_info(void)
{
	k_memset(&k_boot_cpu, 0, sizeof(struct k_cpu_x86));
#if 0

	k_cpu_get_vendor(&k_boot_cpu);
	k_cpu_get_model(&k_boot_cpu);
	k_cpu_get_id(&k_boot_cpu);

	k_cpu_get_max_extended_function(&k_boot_cpu);
	k_cpu_get_processor_name(&k_boot_cpu);

	k_cpu_get_cache_info(&k_boot_cpu);
#endif
}

