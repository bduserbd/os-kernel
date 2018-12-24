#include "include/time/cmos.h"
#include "include/time/clock.h"
#include "include/time/timer.h"
#include "include/time/sleep.h"
#include "include/task/task.h"
#include "include/network/network.h"
#include "include/irq/late-irq.h"
#include "include/video/print.h"
#include "include/user-mode/user-mode.h"

void k_init(void)
{
	k_cmos_init();
	k_timer_init();

	k_sleep(15 * 1000);

/*
	k_task_init();


	k_task_create(k_late_irq_task, NULL);
	k_network_init();

	k_user_mode_init();
*/
}

