#include "kernel/include/acpi/acpica.h"
#include "kernel/include/video/print.h"

void k_acpica_init(void)
{
#if 0
	ACPI_STATUS Status = AE_OK;

	Status = AcpiInitializeSubsystem();
	k_printf("%u\n", Status);
#endif
}

