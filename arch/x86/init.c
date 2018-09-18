#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "include/smbios.h"
#include "kernel/include/acpi/acpi.h"

#ifdef K_CONFIG_SMP
#include "include/smp.h"
#endif

void k_x86_init(void)
{
	k_cpu_get_info();
	k_mp_get_info();
	k_acpi_get_info();
	k_smbios_get_info();

	k_lapic_init();

#ifdef K_CONFIG_SMP
	k_smp_init();
#endif
}

