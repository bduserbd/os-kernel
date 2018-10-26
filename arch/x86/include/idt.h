#ifndef K_IDT_H
#define K_IDT_H

#ifndef __ASSEMBLER__
#include "kernel/include/types.h"
#endif

#define K_IDT_INTERRUPT_GATE_16BIT	0x6
#define K_IDT_INTERRUPT_GATE_32BIT	0xe
#define K_IDT_TRAP_GATE			0xf

#ifndef __ASSEMBLER__

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

#define K_IRQ_MASTER_START	0x20
#define K_IRQ_SLAVE_START	0x28

#ifndef __ASSEMBLER__

void k_idt_init(void);

extern void k_idt_int0(void);
extern void k_idt_int1(void);
extern void k_idt_int2(void);
extern void k_idt_int3(void);
extern void k_idt_int4(void);
extern void k_idt_int5(void);
extern void k_idt_int6(void);
extern void k_idt_int7(void);
extern void k_idt_int8(void);
extern void k_idt_int9(void);
extern void k_idt_int10(void);
extern void k_idt_int11(void);
extern void k_idt_int12(void);
extern void k_idt_int13(void);
extern void k_idt_int14(void);
extern void k_idt_int15(void);
extern void k_idt_int16(void);
extern void k_idt_int17(void);
extern void k_idt_int18(void);
extern void k_idt_int19(void);
extern void k_idt_int20(void);
extern void k_idt_int21(void);
extern void k_idt_int22(void);
extern void k_idt_int23(void);
extern void k_idt_int24(void);
extern void k_idt_int25(void);
extern void k_idt_int26(void);
extern void k_idt_int27(void);
extern void k_idt_int28(void);
extern void k_idt_int29(void);
extern void k_idt_int30(void);
extern void k_idt_int31(void);

extern void k_irq_handler0(void);
extern void k_irq_handler1(void);
extern void k_irq_handler2(void);
extern void k_irq_handler3(void);
extern void k_irq_handler4(void);
extern void k_irq_handler5(void);
extern void k_irq_handler6(void);
extern void k_irq_handler7(void);
extern void k_irq_handler8(void);
extern void k_irq_handler9(void);
extern void k_irq_handler10(void);
extern void k_irq_handler11(void);
extern void k_irq_handler12(void);
extern void k_irq_handler13(void);
extern void k_irq_handler14(void);
extern void k_irq_handler15(void);

extern void k_spurious_int(void);

#endif

#endif

