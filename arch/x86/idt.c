#include "include/idt.h"

void puts(const char *);
void puthex(k_uint32_t);

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

struct k_idt_interrupt_gate k_idt[256] __attribute__((aligned(0x8)));
struct k_idt_register k_idt_reg __attribute__((aligned(0x8)));

struct k_int_regs {
	k_uint32_t edi;
	k_uint32_t esi;
	k_uint32_t ebp;
	k_uint32_t esp;
	k_uint32_t ebx;
	k_uint32_t edx;
	k_uint32_t ecx;
	k_uint32_t eax;
};

void k_int_handler(struct k_int_regs regs)
{
	for (;;)
		asm volatile("hlt");
}

void k_idt_init(void)
{
	int i;

	for (i = 0; i < 32; i++) {
		k_idt[i].offset0 = (k_uint32_t)k_idt_int_arr[i] & 0xffff;
		k_idt[i].offset1 = ((k_uint32_t)k_idt_int_arr[i] >> 16) & 0xffff;
		k_idt[i].segment_selector = 0x8;
		k_idt[i].type = K_IDT_INTERRUPT_GATE_32BIT;
		k_idt[i].dpl = 0;
		k_idt[i].p = 1;
	}

	k_idt_reg.limit = 32 * 8 - 1;
	k_idt_reg.address = (k_uint32_t)&k_idt;

	asm volatile("lidt %0" : : "m" (k_idt_reg));
}

