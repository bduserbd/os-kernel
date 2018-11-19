#include "include/task/task.h"
#include "include/network/network.h"
#include "include/video/print.h"

void foo(void)
{
	while (1) {
		for (int i = 0; i < 1000000; i++)
			asm volatile("nop");

		k_puts("#");
	}
}

void k_init(void)
{
	k_task_init();

	k_task_create(foo);

	asm volatile("sti");

	k_network_init();
}

