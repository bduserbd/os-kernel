#include "include/task/task.h"
#include "include/network/network.h"
#include "include/irq/late-irq.h"
#include "include/video/print.h"

void k_user_mode_init(void (*)(void *));

void foo(void *parameter)
{
	k_printf("From user");
}

void k_init(void)
{
	k_task_init();

	asm volatile("sti");

	k_task_create(k_late_irq_task, NULL);
	k_network_init();

	k_user_mode_init(foo);
}

