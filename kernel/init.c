#include "include/task/task.h"
#include "include/network/network.h"
#include "include/irq/late-irq.h"
#include "include/video/print.h"
#include "include/user-mode/user-mode.h"

void k_init(void)
{
	k_task_init();

	asm volatile("sti");

	k_task_create(k_late_irq_task, NULL);
	k_network_init();

	k_user_mode_init();
}

