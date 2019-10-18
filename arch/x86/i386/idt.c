#include "include/i386/idt.h"
#include "include/i386/registers.h"
#include "include/paging.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

struct k_idt_interrupt_gate k_idt[256] __attribute__((aligned(0x8)));
struct k_idt_register k_idt_reg __attribute__((aligned(0x8)));

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
		k_printf("CR2: %x", k_paging_fault_address());

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

	k_irq_execute_handler(irq);
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

void k_idt_load(void)
{
	asm volatile("lidt %0" : : "m" (k_idt_reg));
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

	k_idt_set_gate(0xff, (k_uint32_t)k_spurious_int, K_IDT_TRAP_GATE_32BIT);

	k_idt_reg.limit = 256 * sizeof(struct k_idt_interrupt_gate) - 1;
	k_idt_reg.address = (k_uint32_t)&k_idt;

	k_idt_load();
}

