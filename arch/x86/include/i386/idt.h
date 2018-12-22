#ifndef K_I386_IDT_H
#define K_I386_IDT_H

#include "kernel/include/types.h"

#define K_IDT_INTERRUPT_GATE_16BIT	0x6
#define K_IDT_INTERRUPT_GATE_32BIT	0xe
#define K_IDT_TRAP_GATE_32BIT		0xf

struct k_idt_register {
	__u16	limit;
	__u32	address;
} __attribute__((packed));

struct k_idt_interrupt_gate {
	__u16	offset0;
	__u16	segment_selector;
	__u8	zero;
	__u8	type : 5;
	__u8	dpl : 2;
	__u8	p : 1;
	__u16	offset1;
} __attribute__((packed));

#endif

