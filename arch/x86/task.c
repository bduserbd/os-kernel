#include "include/registers.h"
#include "kernel/include/task/task.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/string.h"

void *k_task_arch_info_alloc(void)
{
	struct k_registers *regs;

	regs = k_malloc(sizeof(struct k_registers));
	if (!regs)
		return NULL;

	k_memset(regs, 0, sizeof(struct k_registers));

	return regs;
}

