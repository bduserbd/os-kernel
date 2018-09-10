#include "include/cpu.h"

void k_x86_init(void)
{
	*(unsigned char *)0xb8080 = 'O';
	*(unsigned char *)0xb8082 = 'k';

	k_cpu_get_info();
}

