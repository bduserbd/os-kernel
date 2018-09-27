#include "include/idt.h"
#include "kernel/include/video/print.h"

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

extern void k_irq_handler(void);
extern void k_spurious_int(void);

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
	k_uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	k_uint32_t interrupt;
	k_uint32_t error_code, eip, cs, eflags;
} __attribute__((packed));

void k_int_handler(struct k_int_regs regs)
{
	k_puts("EDI: "); k_puthex(regs.edi);
	k_puts("ESI: "); k_puthex(regs.esi);
	k_puts("EBP: "); k_puthex(regs.ebp);
	k_puts("ESP: "); k_puthex(regs.esp);
	k_puts("EBX: "); k_puthex(regs.ebx);
	k_puts("EDX: "); k_puthex(regs.edx);
	k_puts("ECX: "); k_puthex(regs.ecx);
	k_puts("EAX: "); k_puthex(regs.eax);
	k_puts("Interrupt: "); k_puthex(regs.interrupt);
	k_puts("Error Code: "); k_puthex(regs.error_code);
	k_puts("EIP: "); k_puthex(regs.eip);
	k_puts("CS: "); k_puthex(regs.cs);
	k_puts("EFLAGS: "); k_puthex(regs.eflags);

	if (regs.interrupt == 0xe) {
		k_uint32_t cr2;

		asm volatile("mov %%cr2,%0" : "=r" (cr2));

		k_puts("CR2: "); k_puthex(regs.eip);
	}

	for (;;)
		asm volatile("hlt");
}

static void k_idt_set_gate(int i, k_uint32_t offset, int type)
{
	k_idt[i].offset0 = (k_uint16_t)(offset & 0xffff);
	k_idt[i].offset1 = (k_uint16_t)((offset >> 16) & 0xffff);
	k_idt[i].segment_selector = 0x8;
	k_idt[i].type = type;
	k_idt[i].dpl = 0;
	k_idt[i].p = 1;
}

void k_idt_init(void)
{
	int i;

	for (i = 0; i < 32; i++)
		k_idt_set_gate(i , (k_uint32_t)k_idt_int_arr[i], K_IDT_INTERRUPT_GATE_32BIT);

	for (; i < 255; i++)
		k_idt_set_gate(i , (k_uint32_t)k_irq_handler, K_IDT_TRAP_GATE);

	k_idt_set_gate(0xff , (k_uint32_t)k_spurious_int, K_IDT_TRAP_GATE);

	k_idt_reg.limit = 256 * 8 - 1;
	k_idt_reg.address = (k_uint32_t)&k_idt;

	asm volatile("lidt %0" : : "m" (k_idt_reg));
}

