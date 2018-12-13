#ifndef K_CPU_H
#define K_CPU_H

#include "kernel/include/error.h"
#include "kernel/include/types.h"

/* Cache types. */
enum {
	K_CPU_CACHE_TYPE_L1_DATA,
	K_CPU_CACHE_TYPE_L1_INSTRUCTION,
	K_CPU_CACHE_TYPE_L2,
	K_CPU_CACHE_TYPE_L3,
	K_CPU_CACHE_MAX,
};

/* CPUID 0x00000001 */
/* EAX */
#define K_CPUID_BASE_FAMILY(eax)	((eax >> 8) & 0xf)
#define K_CPUID_BASE_MODEL(eax)		((eax >> 4) & 0xf)
#define K_CPUID_STEPPING(eax)		((eax >> 0) & 0xf)
#define K_CPUID_EXT_FAMILY(eax)		((eax >> 20) & 0xff)
#define K_CPUID_EXT_MODEL(eax)		((eax >> 16) & 0xf)

/* EBX */
#define K_CPUID_CLFLUSH_LINE_SIZE(ebx)	(((ebx >> 8) & 0xff) << 3)
#define K_CPUID_INITIAL_APIC_ID(ebx)	((ebx >> 24) & 0xff)

/* EDX */
#define K_CPUID_PSE	(1 << 3)
#define K_CPUID_TSC	(1 << 4)
#define K_CPUID_MSR	(1 << 5)
#define K_CPUID_PAE	(1 << 6)
#define K_CPUID_APIC	(1 << 9)
#define K_CPUID_PGE	(1 << 13)
#define K_CPUID_CLFSH	(1 << 19)

/* CPUID 0x00000002 */
enum {
	K_CPUID2_TYPE_INVALID = 0,
	K_CPUID2_TYPE_GENERAL,
	K_CPUID2_TYPE_TLB,
	K_CPUID2_TYPE_CACHE,
	K_CPUID2_TYPE_DTLB,
	K_CPUID2_TYPE_STLB,
	K_CPUID2_TYPE_PREFETCH,
};

#define K_CPUID2_CACHE_ENTRY(cache, size, way_associative, line_size)	\
	{										\
		.type = K_CPUID2_TYPE_CACHE,						\
		{{									\
			(cache),							\
			(size),								\
			(way_associative),						\
			(line_size),							\
		}},									\
	}
	
struct k_cpuid2_descriptor {
	int type;

	union {
		struct {
			int type;
			unsigned int size;
			int way_associative;
			unsigned int line_size;
		} cache;
	};
};

/* CPUID 0x00000004 */
/* EAX */
#define K_CPUID_CACHE_TYPE_NULL		(0 << 0)
#define K_CPUID_CACHE_TYPE_DATA		(1 << 0)
#define K_CPUID_CACHE_TYPE_INSTRUCTION	(2 << 0)
#define K_CPUID_CACHE_TYPE_UNIFIED	(3 << 0)
#define K_CPUID_CACHE_TYPE(eax)		(eax & 0x1f)

#define K_CPUID_CACHE_LEVEL(eax)	((eax >> 5) & 0x7)

/* EBX */
#define K_CPUID_CACHE_LINE_SIZE(ebx)	((ebx & 0xfff) + 1)
#define K_CPUID_CACHE_PARTITIONS(ebx)	(((ebx >> 12) & 0x3ff) + 1)
#define K_CPUID_CACHE_WAYS(ebx)		(((ebx >> 22) & 0x3ff) + 1)

/* ECX */
#define K_CPUID_CACHE_SETS(ecx)		(ecx + 1)

/* CPUID 0x80000001 */
/* EDX */
#define K_CPUID_LM	(1 << 29)

struct k_cpu_x86 {
	char vendor[13];
	char processor_name[49];

	k_uint32_t max_function, max_extended_function;

	int family;
	int model;
	int stepping;

	unsigned int clflush_line_size;

	struct {
		unsigned int size;
		int way_associative;
		unsigned int line_size;
	} cache[K_CPU_CACHE_MAX];

	k_uint8_t initial_apic_id;
};

extern struct k_cpu_x86 k_boot_cpu;

void k_cpu_get_info(void);
void k_cpu_print_info(struct k_cpu_x86 *);

bool k_cpu_eflag(unsigned long);
void k_cpuid(k_uint32_t, k_uint32_t *, k_uint32_t *, k_uint32_t *, k_uint32_t *);

#endif

