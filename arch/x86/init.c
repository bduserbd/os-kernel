#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "kernel/include/acpi/acpi.h"

void k_x86_init(void)
{
	k_cpu_get_info();

	k_mp_get_info();

	k_acpi_get_info();

	k_lapic_init();

	//*(unsigned char *)0xb8080 = 'O';
	//*(unsigned char *)0xb8082 = 'k';
	//*(unsigned char *)0xb8084 = '!';
}

