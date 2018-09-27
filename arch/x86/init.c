#include "include/init.h"
#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "include/smbios.h"
#include "include/video.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/buddy.h"

#ifdef K_CONFIG_BIOS
void k_x86_init(k_uint32_t heap)
#elif K_CONFIG_UEFI
void k_x86_init(k_uint32_t heap, struct k_fb_info *fb)
#endif
{
	k_cpu_get_info();
	k_mp_get_info();
	k_acpi_get_info();
	k_smbios_get_info();

	k_lapic_init();

#ifdef K_CONFIG_SMP
	k_smp_init();
#endif

	k_buddy_init(heap);

#ifdef K_CONFIG_UEFI
	k_fb_set_info(fb);
	k_shell_init();
#endif
}

