#include "include/x86_64/idt.h"
#include "include/x86_64/registers.h"
#include "include/paging.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

struct k_idt_interrupt_gate k_idt[256] __attribute__((aligned(0x8)));
struct k_idt_register k_idt_reg __attribute__((aligned(0x8)));

static void k_int_print_regs(struct k_int_registers *regs)
{
	k_printf("RAX: %llx RCX: %llx RDX: %llx RBX: %llx\n"
			"RSP: %llx RBP: %llx RSI: %llx RDI: %llx\n"
			"R8: %llx R9: %llx R10: %llx R11: %llx\n"
			"R12: %llx R13: %llx R14: %llx R15: %llx\n"
			"Interrupt: %llx\n"
			"Error code: %llx RIP: %llx CS: %llx RFLAGS: %llx\n",
			regs->rax, regs->rcx, regs->rdx, regs->rbx,
			regs->rsp, regs->rbp, regs->rsi, regs->rdi,
			regs->r8, regs->r9, regs->r10, regs->r11,
			regs->r12, regs->r13, regs->r14, regs->r15,
			regs->interrupt, regs->error_code, regs->rip, regs->cs, regs->rflags);
}

void k_int_handler(struct k_int_registers regs)
{
	k_int_print_regs(&regs);

	if (regs.interrupt == 0xe) {
		k_printf("CR2: %llx", k_paging_fault_address());

		for (;;)
			asm volatile("hlt");
	}
}

void k_irq_handler(struct k_int_registers regs)
{
	unsigned int irq;

	irq = k_irq_from_int(regs.interrupt);
	if (irq == K_INVALID_IRQ)
		return;

	k_irq_ack(irq);

	static volatile int ticks = 0;

	ticks++;
	if (ticks == 10000) {
		k_printf("%x", regs.interrupt - 32);
		ticks = 0;
	}
#if 0
	if (irq == 0)
		k_irq_execute_handler_custom(irq, &regs);
	else
		k_irq_execute_handler(irq);
#endif
}

static void k_idt_set_gate(int i, k_uint64_t offset, int type)
{
	k_idt[i].segment_selector = 0x8;

	k_idt[i].offset0 = (k_uint16_t)(offset & 0xffff);
	k_idt[i].offset1 = (k_uint16_t)((offset >> 16) & 0xffff);
	k_idt[i].offset2 = (k_uint32_t)((offset >> 32) & 0xffffffff);

	k_idt[i].zero = 0;

	k_idt[i].type = type;
	k_idt[i].dpl = 0;
	k_idt[i].p = 1;
}

void k_idt_init(void)
{
	int i, j;

	for (i = 0; i < 32; i++)
		k_idt_set_gate(i, (k_uint64_t)k_idt_int_arr[i], K_IDT_INTERRUPT_GATE_64BIT);

	for (i = K_IRQ_MASTER_START, j = 0; i < K_IRQ_MASTER_START + 8; i++, j++)
		k_idt_set_gate(i, (k_uint64_t)k_irq_handler_arr[j], K_IDT_INTERRUPT_GATE_64BIT);

	for (i = K_IRQ_SLAVE_START, j = 8; i < K_IRQ_SLAVE_START + 8; i++, j++)
		k_idt_set_gate(i, (k_uint64_t)k_irq_handler_arr[j], K_IDT_INTERRUPT_GATE_64BIT);

	k_idt_reg.limit = 256 * sizeof(struct k_idt_interrupt_gate) - 1;
	k_idt_reg.address = (k_uint64_t)&k_idt;

	asm volatile("lidtq %0" : : "m" (k_idt_reg));
}

