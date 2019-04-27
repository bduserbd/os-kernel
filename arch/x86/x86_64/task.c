#include "include/x86_64/registers.h"
#include "kernel/include/task/task.h"
#include "kernel/include/mm/mm.h"

void *k_task_arch_info_alloc(k_task_entry_point_t func, void *stack, void *parameter)
{
	struct k_registers *regs;

	regs = k_malloc(sizeof(struct k_registers));
	if (!regs)
		return NULL;

	k_memset(regs, 0, sizeof(struct k_registers));

	if (func) {
		regs->rip = (k_uint64_t)func;

		k_uint64_t rsp = ((k_uint64_t)stack & ~0xfULL) - sizeof(k_uint64_t);

		*(k_uint64_t *)rsp = regs->rip;
		*(k_uint64_t *)(rsp - 0x48) = (k_uint64_t)parameter;

		regs->rsp = rsp - 16 * sizeof(k_uint64_t);
	}

	return regs;
}

void k_task_arch_set_new_context(void *, void *);

void k_task_arch_switch_context(struct k_task *a, struct k_task *b)
{
	k_task_arch_set_new_context(a->arch, b->arch);
}

