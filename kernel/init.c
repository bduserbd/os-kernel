#include "include/task/task.h"
#include "include/network/network.h"

void k_init(void)
{
	k_task_init();
	k_network_init();
}

