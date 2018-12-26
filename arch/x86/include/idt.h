#ifndef K_IDT_H
#define K_IDT_H

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

extern void k_system_call(void);

void (*k_idt_int_arr[32])(void);
void (*k_irq_handler_arr[16])(void);

#endif

#endif

