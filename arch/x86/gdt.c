#include "include/gdt.h"
#include "include/tss.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/string.h"

struct k_gdt_entry k_gdt[6] __attribute__((aligned(0x8)));
struct k_gdt_register k_gdt_reg __attribute__((aligned(0x8)));

struct k_tss k_user_tss __attribute__((aligned(0x8)));

static inline void k_gdt_set_entry(struct k_gdt_entry *entry, unsigned base, unsigned limit,
		unsigned type, unsigned s, unsigned dpl,
		unsigned db, unsigned g)
{
	entry->base0 = (base & 0xffff);
	entry->base1 = (base >> 16) & 0xff;
	entry->base2 = (base >> 24) & 0xff;

	entry->limit0 = (limit & 0xffff);
	entry->limit1 = (limit >> 16) & 0xf;

	entry->type = type;
	entry->s = s;
	entry->dpl = dpl;
	entry->p = 1;

	entry->db = db;
	entry->g = g;
}

static inline void k_gdt_set_kernel_entry(struct k_gdt_entry *entry, unsigned base, unsigned limit,
		unsigned type)
{
	k_gdt_set_entry(entry, base, limit, type, 1, 0, 1, 1);
}

static inline void k_gdt_set_user_entry(struct k_gdt_entry *entry, unsigned base, unsigned limit,
		unsigned type)
{
	k_gdt_set_entry(entry, base, limit, type, 1, 3, 1, 1);
}

static inline void k_gdt_set_tss_entry(struct k_gdt_entry *entry)
{
	k_gdt_set_entry(entry, (unsigned long)&k_user_tss, sizeof(struct k_tss),
			0x9, 0, 3, 0, 0);
}

void k_gdt_init(void)
{
	k_memset(&k_gdt[0], 0, sizeof(struct k_gdt_entry));

	k_gdt_set_kernel_entry(&k_gdt[1], 0, 0xfffff, 0xa);
	k_gdt_set_kernel_entry(&k_gdt[2], 0, 0xfffff, 0x2);

	k_gdt_set_user_entry(&k_gdt[3], 0, 0xfffff, 0xa);
	k_gdt_set_user_entry(&k_gdt[4], 0, 0xfffff, 0x2);

	k_gdt_set_tss_entry(&k_gdt[5]);

	k_gdt_reg.limit = sizeof(k_gdt) - 1;
	k_gdt_reg.address = (k_uint32_t)&k_gdt;

	asm volatile("lgdt %0" : : "m" (k_gdt_reg));

	asm volatile(
		"mov	%0, %%ax	\n"
		"ltr	%%ax"
		: :
		"i" (K_GDT_TSS_SELECTOR | 3)
	);
}

k_error_t k_tss_init(void)
{
	void *stack;

	stack = k_buddy_alloc(0x1000);
	if (!stack)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_user_tss.ss0 = K_GDT_KERNEL_DATA_SELECTOR;
	k_user_tss.esp0 = (k_uint32_t)stack;

	return K_ERROR_NONE;
}

