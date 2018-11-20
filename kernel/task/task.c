#include "include/task/task.h"
#include "kernel/include/mm/mm.h"

struct k_task *k_task = NULL;

void *k_task_arch_info_alloc(k_task_entry_point_t, void *, void *);
void k_task_arch_switch_context(struct k_task *, void *, struct k_task *);

static k_pid_t k_task_get_free_pid(void)
{
	static k_pid_t k_pid_counter = K_TASK_FIRST_PID + 1;

	return k_pid_counter++;
}

static k_error_t k_task_main(void *parameter)
{
	k_task->func(parameter);

	for(;;) ;

	return K_ERROR_FATAL;
}

void k_task_create(k_task_entry_point_t func, void *parameter)
{
	struct k_task *task, *temp;

	if (!k_task)
		return;

	if (!func)
		return;

	task = k_malloc(sizeof(struct k_task));
	if (!task)
		return;

	task->pid = k_task_get_free_pid();
	task->state = K_TASK_STATE_SLEEPING;

	task->func = func;

	task->stack = k_buddy_alloc(K_TASK_STACK_SIZE);
	if (!task->stack)
		return;

	task->arch = k_task_arch_info_alloc(k_task_main, task->stack, parameter);
	if (!task->arch)
		return;

	temp = k_task->next;
	k_task->next = task;
	task->next = temp;
}

void k_task_switch(void *context)
{
	struct k_task *a, *b;

	if (!k_task)
		return;

	if (k_task == k_task->next)
		return;

	a = k_task;
	b = k_task->next;

	k_task = k_task->next;

	k_task_arch_switch_context(a, context, b);
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

	k_task->arch = k_task_arch_info_alloc(NULL, NULL, NULL);
	if (!k_task->arch)
		return;

	k_task->func = NULL;
	k_task->stack = __k_stack_start; // Unneeded.

	k_task->next = k_task;
}

