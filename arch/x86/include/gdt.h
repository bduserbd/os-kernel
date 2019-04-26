#ifndef K_GDT_H
#define K_GDT_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#include "kernel/include/error.h"
#endif

#define K_GDT_KERNEL_CODE_SELECTOR	0x8
#define K_GDT_KERNEL_DATA_SELECTOR	0x10
#define K_GDT_USER_CODE_SELECTOR	0x18
#define K_GDT_USER_DATA_SELECTOR	0x20
#define K_GDT_TSS_SELECTOR		0x28

#ifndef __ASSEMBLER__

struct k_gdt_register {
	__u16	limit;
	__u32	address;
} __attribute__((packed));

struct k_gdt_entry {
	__u16	limit0;
	__u16	base0;
	__u8	base1;
	__u8	type : 4;
	__u8	s : 1;
	__u8	dpl : 2;
	__u8	p : 1;
	__u8	limit1 : 4;
	__u8	l : 1;
	__u8	avl : 1;
	__u8	db : 1;
	__u8	g : 1;
	__u8	base2;
} __attribute__((packed));

void k_gdb_load(void);
void k_gdt_init(void);
k_error_t k_tss_init(void);

#endif

#endif

