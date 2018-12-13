#include "include/idt.h"
#include "include/registers.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/io/io.h"
#include "kernel/include/video/print.h"

struct k_idt_interrupt_gate k_idt[256] __attribute__((aligned(0x8)));
struct k_idt_register k_idt_reg __attribute__((aligned(0x8)));

static void (*k_idt_int_arr[32])(void) = {
	k_idt_int0, k_idt_int1, k_idt_int2, k_idt_int3,
	k_idt_int4, k_idt_int5, k_idt_int6, k_idt_int7,
	k_idt_int8, k_idt_int9, k_idt_int10, k_idt_int11,
	k_idt_int12, k_idt_int13, k_idt_int14, k_idt_int15,
	k_idt_int16, k_idt_int17, k_idt_int18, k_idt_int19,
	k_idt_int20, k_idt_int21, k_idt_int22, k_idt_int23,
	k_idt_int24, k_idt_int25, k_idt_int26, k_idt_int27,
	k_idt_int28, k_idt_int29, k_idt_int30, k_idt_int31,
};

static void (*k_irq_handler_arr[16])(void) = {
	k_irq_handler0, k_irq_handler1, k_irq_handler2, k_irq_handler3,
	k_irq_handler4, k_irq_handler5, k_irq_handler6, k_irq_handler7,
	k_irq_handler8, k_irq_handler9, k_irq_handler10, k_irq_handler11,
	k_irq_handler12, k_irq_handler13, k_irq_handler14, k_irq_handler15,
};

static void k_int_print_regs(struct k_int_registers *regs)
{
	k_printf("EDI: %x ESI: %x EBP: %x ESP: %x\n"
			"EBX: %x EDX: %x ECX: %x EAX: %x\n"
			"Interrupt: %x\n"
			"Error Code: %x EIP: %x CS: %x EFLAGS: %x\n",
			regs->edi, regs->esi, regs->ebp, regs->esp,
			regs->ebx, regs->edx, regs->ecx, regs->eax,
			regs->interrupt, regs->error_code, regs->eip, regs->cs, regs->eflags);
}

void k_int_handler(struct k_int_registers regs)
{
	k_int_print_regs(&regs);

	if (regs.interrupt == 0xe) {
		k_uint32_t cr2;

		asm volatile("mov %%cr2,%0" : "=r" (cr2));

		k_printf("CR2: %x", cr2);

		for (;;)
			asm volatile("hlt");
	}
}

void k_irq_handler(struct k_int_registers regs)
{
	k_printf("%d", regs.interrupt - 32);
	k_lapic_eoi();
#if 0
	unsigned int irq;

	irq = k_irq_from_int(regs.interrupt);
	if (irq == K_INVALID_IRQ)
		return;

	k_irq_ack(irq);

	if (irq == 0)
		k_irq_execute_handler_custom(irq, &regs);
	else
		k_irq_execute_handler(irq);
#endif
}

static void k_idt_set_gate(int i, k_uint32_t offset, int type)
{
	k_idt[i].segment_selector = 0x8;
	k_idt[i].offset0 = (k_uint16_t)(offset & 0xffff);
	k_idt[i].offset1 = (k_uint16_t)((offset >> 16) & 0xffff);

	k_idt[i].zero = 0;

	k_idt[i].type = type;
	k_idt[i].dpl = 0;
	k_idt[i].p = 1;
}

static void k_idt_set_user_gate(int i, k_uint32_t offset, int type)
{
	k_idt_set_gate(i, offset, type);
	k_idt[i].dpl = 3;
}

void k_idt_init(void)
{
	int i, j;

	for (i = 0; i < 32; i++)
		k_idt_set_gate(i, (k_uint32_t)k_idt_int_arr[i], K_IDT_INTERRUPT_GATE_32BIT);

	for (i = K_IRQ_MASTER_START, j = 0; i < K_IRQ_MASTER_START + 8; i++, j++)
		k_idt_set_gate(i, (k_uint32_t)k_irq_handler_arr[j], K_IDT_INTERRUPT_GATE_32BIT);

	for (i = K_IRQ_SLAVE_START, j = 8; i < K_IRQ_SLAVE_START + 8; i++, j++)
		k_idt_set_gate(i, (k_uint32_t)k_irq_handler_arr[j], K_IDT_INTERRUPT_GATE_32BIT);

	k_idt_set_user_gate(0x80, (k_uint32_t)k_system_call, K_IDT_INTERRUPT_GATE_32BIT);

	k_idt_set_gate(0xff, (k_uint32_t)k_spurious_int, K_IDT_TRAP_GATE);

	k_idt_reg.limit = 256 * 8 - 1;
	k_idt_reg.address = (k_uint32_t)&k_idt;

	asm volatile("lidt %0" : : "m" (k_idt_reg));
}

