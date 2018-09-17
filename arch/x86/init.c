#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "include/smp.h"
#include "kernel/include/acpi/acpi.h"

void k_x86_init(void)
{
	k_cpu_get_info();
	k_mp_get_info();
	k_acpi_get_info();

	k_lapic_init();
	k_smp_init();
}

