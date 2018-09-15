#ifndef K_IDT_H
#define K_IDT_H

#include "include/types.h"

#define K_IDT_INTERRUPT_GATE_16BIT	0x6
#define K_IDT_INTERRUPT_GATE_32BIT	0xe

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

void k_idt_init(void);

#endif

