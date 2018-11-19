#include "include/registers.h"
#include "kernel/include/task/task.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/string.h"

void *k_task_arch_info_alloc(void *callback, void *stack)
{
	struct k_registers *regs;

	regs = k_malloc(sizeof(struct k_registers));
	if (!regs)
		return NULL;

	k_memset(regs, 0, sizeof(struct k_registers));

	regs->eip = (k_uint32_t)callback;
	regs->esp = (k_uint32_t)stack;

	return regs;
}

static void k_task_arch_save_context(struct k_task *task, struct k_int_registers *regs)
{
	((struct k_registers *)task->arch)->eip = regs->eip;

	((struct k_registers *)task->arch)->ebp = regs->ebp;
	((struct k_registers *)task->arch)->esp = regs->esp;

	((struct k_registers *)task->arch)->eflags = regs->eflags;

	((struct k_registers *)task->arch)->eax = regs->eax;
	((struct k_registers *)task->arch)->ebx = regs->ebx;
	((struct k_registers *)task->arch)->ecx = regs->ecx;
	((struct k_registers *)task->arch)->edx = regs->edx;

	((struct k_registers *)task->arch)->edi = regs->edi;
	((struct k_registers *)task->arch)->esi = regs->esi;
}

static void k_task_arch_set_new_context(struct k_task *task, struct k_int_registers *regs)
{
	regs->eip = ((struct k_registers *)task->arch)->eip;

	regs->ebp = ((struct k_registers *)task->arch)->ebp;
	regs->esp = ((struct k_registers *)task->arch)->esp;

	regs->eflags = ((struct k_registers *)task->arch)->eflags;

	regs->eax = ((struct k_registers *)task->arch)->eax;
	regs->ebx = ((struct k_registers *)task->arch)->ebx;
	regs->ecx = ((struct k_registers *)task->arch)->ecx;
	regs->edx = ((struct k_registers *)task->arch)->edx;

	regs->edi = ((struct k_registers *)task->arch)->edi;
	regs->esi = ((struct k_registers *)task->arch)->esi;
}

/* Right now, the context here is the pointer to the registers pushed
   in interrupt handler. */
void k_task_arch_switch_context(struct k_task *task, void *context,
		struct k_task *next_task)
{
	struct k_int_registers *regs;

	regs = context;

	k_task_arch_save_context(task, regs);
	k_task_arch_set_new_context(next_task, regs);
}
