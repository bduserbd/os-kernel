#include "include/task/task.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/spinlock.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/data-structures/heap/binary-heap.h"

extern int k_cpus_count;

static bool k_task_is_init = false;

struct k_task_cpu_info {
	struct k_task *current;

	struct k_binary_heap *ready;
};

static struct k_task_cpu_info *k_task_cpu_info = NULL;

static k_spin_lock_t k_task_lock;

static k_pid_t k_task_get_free_pid(void)
{
	static k_pid_t k_pid_counter = K_TASK_FIRST_PID + 1;

	return k_pid_counter++;
}

struct k_task *k_task_get_current(void)
{
	return k_task_cpu_info[k_get_cpu_number()].current;	
}

static k_error_t k_task_main(void *parameter)
{
	k_spin_unlock(&k_task_lock);

	k_task_get_current()->func(parameter);

	for (;;) ;

	return K_ERROR_FATAL;
}

void k_task_arch_switch_context(struct k_task *, struct k_task *);

static struct k_task *k_task_get_next_schedule()
{
	struct k_task *task;

	do {
		
	} while (!task);

	return task;
}

void k_task_switch(void)
{
	struct k_task *a, *b;

	if (!k_task_is_init)
		return;

	if (k_spin_is_locked(&k_task_lock))
		return;

	k_spin_lock(&k_task_lock);

	k_printf("L%d ", k_lapic_id());

	a = k_task;
	a->state = K_TASK_STATE_SLEEPING;

	b = a->next;
	b->state = K_TASK_STATE_RUNNING;

	k_task_arch_switch_context(a, b);

_exit:
	k_spin_unlock(&k_task_lock);
}

void k_schedule(void)
{
	k_task_switch();
}

void *k_task_arch_info_alloc(k_task_entry_point_t, void *, void *);
void k_task_arch_info_free(void *);

static struct k_task *k_task_prepare_new(k_task_entry_point_t func, void *parameter)
{
	struct k_task *task;

	if (!func)
		return NULL;

	task = k_malloc(sizeof(struct k_task));
	if (!task)
		goto _exit;

	task->pid = k_task_get_free_pid();
	task->state = K_TASK_STATE_SLEEPING;

	task->func = func;

	task->stack = k_buddy_alloc(K_TASK_STACK_SIZE);
	if (!task->stack)
		goto _exit;

	task->arch = k_task_arch_info_alloc(k_task_main, (k_uint8_t *)task->stack +
			K_TASK_STACK_SIZE - 1, parameter);
	if (!task->arch)
		goto _exit;

	return task;

_exit:
	if (task) {
		if (task->stack)
			k_buddy_free(task->stack);

		if (task->arch)
			k_task_arch_info_free(task->arch);

		k_free(task);
	}

	return NULL;
}

void k_task_create(k_task_entry_point_t func, void *parameter)
{
	k_error_t error;
	struct k_task *task;
	static int next_cpu = 0;

	if (!k_task_is_init)
		return;

	k_spin_lock(&k_task_lock);

	task = k_task_prepare_new(func, parameter);
	if (!task)
		return;

#ifdef K_CONFIG_SMP
	next_cpu++;
	if (next_cpu == k_cpus_count)
		next_cpu = 0;
#endif

	error = k_binary_heap_insert(k_task_cpu_info[next_cpu].ready, task);
	if (error)
		return;

	k_spin_unlock(&k_task_lock);
}

extern __u8 __k_stack_start[];

static struct k_task *k_task_first_alloc(void)
{
	struct k_task *task;

	if (k_task_is_init)
		return NULL;

	task = k_malloc(sizeof(struct k_task));
	if (!task)
		return NULL;

	task->pid = K_TASK_FIRST_PID;
	task->state = K_TASK_STATE_RUNNING;

	task->arch = k_task_arch_info_alloc(NULL, NULL, NULL);
	if (!task->arch) {
		k_free(task);

		return NULL;
	}

	task->func = NULL;
	task->stack = __k_stack_start; // Unneeded.

	return task;
}

static int k_task_compare(void *ptr1, void *ptr2)
{
	struct k_task *task1, *task2;

	task1 = ptr1;
	task2 = ptr2;

	if (task1->priority < task2->priority)
		return 1;
	else if (task1->priority > task2->priority)
		return -1;
	else
		return 0;
}

static k_error_t k_task_info_alloc(void)
{
	int i;

	if (k_task_is_init)
		return K_ERROR_FAILURE;

	k_task_cpu_info = k_malloc(k_cpus_count * sizeof(struct k_task_cpu_info));
	if (!k_task_cpu_info)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	for (i = 0; i < k_cpus_count; i++) {
		k_task_cpu_info[i].current = NULL;

		// TODO: Maybe increase heap size. Free when failure ?
		k_task_cpu_info[i].ready = k_binary_heap_init(K_BINARY_HEAP_MAX, 0xff,
			k_task_compare);
		if (!k_task_cpu_info[i].ready)
			return K_ERROR_FAILURE;
	}

	return K_ERROR_NONE;
}

void k_task_init(void)
{
	k_error_t error;
	struct k_task *task;

	if (k_task_is_init)
		return;

	k_spin_lock(&k_task_lock);

	task = k_task_first_alloc();
	if (!task)
		return;

	error = k_task_info_alloc();
	if (error)
		return;

	k_task_cpu_info[k_get_cpu_number()].current = task;

	k_task_is_init = true;

	k_spin_unlock(&k_task_lock);
}

