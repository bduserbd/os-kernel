#include "include/registers.h"
#include "kernel/include/video/print.h"

static void k_system_call_print_regs(struct k_system_call_registers *regs)
{
        k_printf("EDI: %x ESI: %x EBP: %x ESP: %x\n"
                        "EBX: %x EDX: %x ECX: %x EAX: %x\n"
                        "EIP: %x CS: %x EFLAGS: %x ESP: %x SS: %x\n",
                        regs->edi, regs->esi, regs->ebp, regs->_esp,
                        regs->ebx, regs->edx, regs->ecx, regs->eax,
                        regs->eip, regs->cs, regs->eflags, regs->esp, regs->ss);
}

void k_system_call_handler(struct k_system_call_registers regs)
{
	k_system_call_print_regs(&regs);
}

