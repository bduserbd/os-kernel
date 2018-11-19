#include "include/task/task.h"
#include "kernel/include/mm/mm.h"

struct k_task *k_task = NULL;

void *k_task_arch_info_alloc(void *, void *);
void k_task_arch_switch_context(struct k_task *, void *, struct k_task *);

static k_pid_t k_task_get_free_pid(void)
{
	static k_pid_t k_pid_counter = K_TASK_FIRST_PID + 1;

	return k_pid_counter++;
}

void k_task_create(void *callback)
{
	struct k_task *task, *temp;

	if (!k_task)
		return;

	task = k_malloc(sizeof(struct k_task));
	if (!task)
		return;

	task->pid = k_task_get_free_pid();
	task->state = K_TASK_STATE_SLEEPING;

	task->stack = k_buddy_alloc(K_TASK_STACK_SIZE);
	if (!task->stack)
		return;

	task->arch = k_task_arch_info_alloc(callback, task->stack);
	if (!task->arch)
		return;

	temp = k_task->next;
	k_task->next = task;
	task->next = temp;
}

void k_task_switch(void *context)
{
	if (!k_task)
		return;

	//if (k_task == k_task->next)
	//	return;

	k_task_arch_switch_context(k_task, context, k_task->next);

	//k_printf("%x ", k_task);
}

extern __u8 __k_stack_start[];

void k_task_init(void)
{
	if (k_task)
		return;

	k_task = k_malloc(sizeof(struct k_task));
	if (!k_task)
		return;

	k_task->pid = K_TASK_FIRST_PID;
	k_task->state = K_TASK_STATE_RUNNING;

	k_task->arch = k_task_arch_info_alloc(NULL, NULL);
	if (!k_task->arch)
		return;

	k_task->stack = __k_stack_start;

	k_task->next = k_task;
}

