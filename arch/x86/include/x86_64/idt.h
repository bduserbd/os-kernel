#ifndef K_X86_64_IDT_H
#define K_X86_64_IDT_H

#include "kernel/include/types.h"
#include "arch/x86/include/idt.h"

#define K_IDT_INTERRUPT_GATE_64BIT	0xe
#define K_IDT_TRAP_GATE_64BIT		0xf

struct k_idt_register {
	__u16	limit;
	__u64	address;
} __attribute__((packed));

struct k_idt_interrupt_gate {
	__u16	offset0;
	__u16	segment_selector;
	__u8	zero;
	__u8	type : 5;
	__u8	dpl : 2;
	__u8	p : 1;
	__u16	offset1;
	__u32	offset2;
	__u32	reserved;
} __attribute__((packed));

#endif

