#include "include/idt.h"

void (*k_idt_int_arr[32])(void) = {
	k_idt_int0, k_idt_int1, k_idt_int2, k_idt_int3,
	k_idt_int4, k_idt_int5, k_idt_int6, k_idt_int7,
	k_idt_int8, k_idt_int9, k_idt_int10, k_idt_int11,
	k_idt_int12, k_idt_int13, k_idt_int14, k_idt_int15,
	k_idt_int16, k_idt_int17, k_idt_int18, k_idt_int19,
	k_idt_int20, k_idt_int21, k_idt_int22, k_idt_int23,
	k_idt_int24, k_idt_int25, k_idt_int26, k_idt_int27,
	k_idt_int28, k_idt_int29, k_idt_int30, k_idt_int31,
};

void (*k_irq_handler_arr[16])(void) = {
	k_irq_handler0, k_irq_handler1, k_irq_handler2, k_irq_handler3,
	k_irq_handler4, k_irq_handler5, k_irq_handler6, k_irq_handler7,
	k_irq_handler8, k_irq_handler9, k_irq_handler10, k_irq_handler11,
	k_irq_handler12, k_irq_handler13, k_irq_handler14, k_irq_handler15,
};

