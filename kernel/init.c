#include "include/task/task.h"
#include "include/network/network.h"
#include "include/video/print.h"

k_error_t foo(void *parameter)
{
	k_printf("%x\n", parameter);

	while (1) {
		for (int i = 0; i < 100000000; i++)
			asm volatile("nop");

		k_puts("#");
	}

	return K_ERROR_NONE;
}

void k_init(void)
{
	k_task_init();

	asm volatile("sti");

	k_task_create(foo, (void *)0xaabbccdd);

	k_network_init();
}

