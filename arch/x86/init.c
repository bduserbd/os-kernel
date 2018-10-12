#include "include/init.h"
#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "include/smbios.h"
#include "include/video.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/buddy.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/video/print.h"
#include "kernel/include/initramfs/initramfs.h"

void k_x86_init(k_uint32_t heap, struct k_fb_info *fb, void *rsdp,
		k_uint32_t initramfs_start, k_uint32_t initramfs_length)
{
	k_buddy_init(heap);

#ifdef K_CONFIG_BIOS
	k_text_set_info(fb);
	k_print_set_output_callback(k_text_puts);
#elif K_CONFIG_UEFI
	k_fb_set_info(fb);
	k_shell_init();
	k_print_set_output_callback(k_shell_puts);
#endif

	k_cpu_get_info();

	k_slab_init();

#if 0
	k_mp_get_info();
	k_acpi_get_info(rsdp);
	k_smbios_get_info();

	k_lapic_init();

#ifdef K_CONFIG_SMP
	k_smp_init();
#endif

	k_initramfs_get_info(initramfs_start, initramfs_length);
#endif
}

