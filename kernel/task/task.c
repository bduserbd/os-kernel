#include "include/task/task.h"
#include "kernel/include/mm/mm.h"

struct k_task *k_task = NULL;

void *k_task_arch_info_alloc(k_task_entry_point_t, void *, void *);
void k_task_arch_switch_context(struct k_task *, struct k_task *);

static k_pid_t k_task_get_free_pid(void)
{
	static k_pid_t k_pid_counter = K_TASK_FIRST_PID + 1;

	return k_pid_counter++;
}

static int k_irq_disable_counter = 0;

static void k_schedule_lock(void)
{
	asm volatile("cli");
	k_irq_disable_counter++;
}

static void k_schedule_unlock(void)
{
	k_irq_disable_counter--;
	if (k_irq_disable_counter == 0)
		asm volatile("sti");
}

static k_error_t k_task_main(void *parameter)
{
	k_schedule_unlock();
	k_task->func(parameter);

	for(;;) ;

	return K_ERROR_FATAL;
}

void k_task_switch(void)
{
	struct k_task *a, *b;

	if (!k_task)
		return;

	if (k_task == k_task->next)
		return;

	a = k_task;
	a->state = K_TASK_STATE_SLEEPING;

	b = a->next;
	b->state = K_TASK_STATE_RUNNING;

	k_task = k_task->next;

	k_task_arch_switch_context(a, b);
}

void k_schedule(void)
{
	k_schedule_lock();
	k_task_switch();
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

	task->arch = k_task_arch_info_alloc(k_task_main, (k_uint8_t *)task->stack +
			K_TASK_STACK_SIZE - 1, parameter);
	if (!task->arch)
		return;

	temp = k_task->next;
	k_task->next = task;
	task->next = temp;
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

