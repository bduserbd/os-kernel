#include "include/system-call.h"
#include "include/i386/registers.h"
#include "kernel/include/video/print.h"

static void k_system_call_exit(struct k_system_call_registers *regs)
{

}

static void k_system_call_fork(struct k_system_call_registers *regs)
{

}

static void k_system_call_read(struct k_system_call_registers *regs)
{

}

static void k_system_call_write(struct k_system_call_registers *regs)
{

}

static void k_system_call_open(struct k_system_call_registers *regs)
{

}

static void k_system_call_close(struct k_system_call_registers *regs)
{

}

static void k_system_call_getpid(struct k_system_call_registers *regs)
{

}

static void (*k_system_call_array[K_SYSTEM_CALL_TOTAL])(struct k_system_call_registers *) = {
	[K_SYSTEM_CALL_EXIT] = k_system_call_exit,
	[K_SYSTEM_CALL_FORK] = k_system_call_fork,
	[K_SYSTEM_CALL_READ] = k_system_call_read,
	[K_SYSTEM_CALL_WRITE] = k_system_call_write,
	[K_SYSTEM_CALL_OPEN] = k_system_call_open,
	[K_SYSTEM_CALL_CLOSE] = k_system_call_close,
	[K_SYSTEM_CALL_GETPID] = k_system_call_getpid,
};

static void k_system_call_print_regs(struct k_system_call_registers *regs)
{
        k_printf("EDI: %x ESI: %x EBP: %x KERNEL ESP: %x\n"
                        "EBX: %x EDX: %x ECX: %x EAX: %x\n"
                        "EIP: %x CS: %x EFLAGS: %x USER ESP: %x SS: %x\n",
                        regs->edi, regs->esi, regs->ebp, regs->kernel_esp,
                        regs->ebx, regs->edx, regs->ecx, regs->eax,
                        regs->eip, regs->cs, regs->eflags, regs->user_esp, regs->ss);
}

void k_system_call_handler(struct k_system_call_registers regs)
{
	k_system_call_print_regs(&regs);

	if (!k_system_call_array[regs.eax])
		return;

	k_system_call_array[regs.eax](&regs);
}

