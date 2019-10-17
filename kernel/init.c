#include "include/time/cmos.h"
#include "include/time/clock.h"
#include "include/time/timer.h"
#include "include/time/sleep.h"
#include "include/modules/loader.h"
#include "include/initramfs/initramfs.h"
#include "include/acpi/acpica.h"
#include "include/task/task.h"
#include "include/network/network.h"
#include "include/irq/late-irq.h"
#include "include/video/print.h"
#include "include/user-mode/user-mode.h"

extern unsigned long k_initramfs_start;
extern unsigned long k_initramfs_length;

k_error_t foo1(void *param)
{
	while (1) {
		//k_printf("B%u ", (unsigned long)param);
		//for (int i = 0; i < 10000; i++) asm volatile("nop");
	}
}

k_error_t foo2(void *param)
{
	while (1) {
		//k_printf("C%u ", (unsigned long)param);
		//for (int i = 0; i < 10000; i++) asm volatile("nop");
	}
}

k_error_t foo3(void *param)
{
	while (1) {
		//k_printf("D%u ", (unsigned long)param);
		//for (int i = 0; i < 10000; i++) asm volatile("nop");
	}
}

void k_init(void)
{
	k_cmos_init();
	k_clock_init();
	k_timer_init();

	k_task_init();

	k_task_create(foo3, (void *)3);
	k_task_create(foo2, (void *)2);
	k_task_create(foo1, (void *)1);

	while (1) {
		//k_printf("A0 ");
		//for (int i = 0; i < 10000; i++) asm volatile("nop");
	}


/*
	k_acpica_init();

	k_loader_init();
	k_initramfs_get_info(k_initramfs_start, k_initramfs_length);

	while (1) {
		k_printf("#");
		k_sleep(1000);
	}

	k_task_create(k_late_irq_task, NULL);
	k_network_init();

	k_user_mode_init();
*/
}

