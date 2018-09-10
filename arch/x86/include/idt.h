#ifndef K_IDT_H
#define K_IDT_H

#include "include/types.h"

enum {
	K_IDT_INTERRUPT_GATE_16BIT	= 0x6,
	K_IDT_INTERRUPT_GATE_32BIT	= 0xe,
};

#define K_IDT_INTERRUPT_GATE(func)			\
	{						\
		.offset0 = func & 0xffff,		\
		.segment_selector = 0,			\
		.zero = 0,				\
		.type = K_IDT_INTERRUPT_GATE_32BIT,	\
		.dp = 0,				\
		.p = 1,					\
		.offset1 = 0,				\
	},

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

