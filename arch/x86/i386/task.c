#include "include/i386/registers.h"
#include "kernel/include/task/task.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/string.h"

void *k_task_arch_info_alloc(k_task_entry_point_t func, void *stack, void *parameter)
{
	struct k_registers *regs;

	regs = k_malloc(sizeof(struct k_registers));
	if (!regs)
		return NULL;

	k_memset(regs, 0, sizeof(struct k_registers));

	if (func) {
		regs->eip = (k_uint32_t)func;

		k_uint32_t esp = ((k_uint32_t)stack & ~0xfUL) - sizeof(k_uint32_t);

		*(k_uint32_t *)(esp-=sizeof(k_uint32_t)) = (k_uint32_t)parameter;
		*(k_uint32_t *)(esp-=sizeof(k_uint32_t)) = 0x0;
		*(k_uint32_t *)(esp-=sizeof(k_uint32_t)) = regs->eip;

		regs->esp = esp - 8 * sizeof(k_uint32_t);
	}

	return regs;
}

void k_task_arch_set_new_context(void *, void *);

void k_task_arch_switch_context(struct k_task *task, struct k_task *next_task)
{
	k_task_arch_set_new_context(task->arch, next_task->arch);
}

