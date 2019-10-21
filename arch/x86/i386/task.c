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
		//*(k_uint32_t *)(esp-=sizeof(k_uint32_t)) = regs->eip;

		//regs->esp = esp - 8 * sizeof(k_uint32_t);
		regs->esp = esp;
	}

	return regs;
}

void k_task_arch_info_free(void *arch)
{
	if (arch) {
		k_memset(arch, 0, sizeof(struct k_registers));
		k_free(arch);
	}
}

void k_task_arch_set_new_context(k_uint32_t *, k_uint32_t *, k_uint32_t, k_uint32_t);

void k_task_arch_switch_context(struct k_task *a, struct k_task *b)
{
	struct k_registers *bregs;

	bregs = b->arch;

	if (a) {
		struct k_registers *aregs;

		aregs = a->arch;

		k_task_arch_set_new_context(&aregs->eip, &aregs->esp, bregs->eip,
			bregs->esp);
	} else {
		k_uint32_t dummy;

		k_task_arch_set_new_context(&dummy, &dummy, bregs->eip, bregs->esp);
	}
}

