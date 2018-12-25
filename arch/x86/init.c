#include "include/init.h"
#include "include/cpu.h"
#include "include/lapic.h"
#include "include/ioapic.h"
#include "include/hpet.h"
#include "include/smbios.h"
#include "include/8259a.h"
#include "include/8253.h"
#include "include/video.h"
#include "include/paging.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/modules/loader.h"
#include "kernel/include/initramfs/initramfs.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

void k_init(void);

void k_x86_init(void *smbios, void *rsdp,
		k_uint32_t initramfs_start, k_uint32_t initramfs_length)
{
	k_error_t error;

	k_cpu_get_info();

	k_slab_init();

	k_memory_zone_init(k_normal_frames, 0, k_total_normal_frames);
	error = k_reserve_reserved_pages();
	if (error)
		return;

	//k_cpu_print_info(&k_boot_cpu);

#ifdef K_CONFIG_BIOS
	k_mp_get_info();
#endif

#if 0
	k_printf("%lx ", k_malloc(4));
	k_printf("%lx ", k_malloc(7));
	k_printf("%lx ", k_malloc(12));
	k_printf("%lx ", k_malloc(12));
	k_printf("%lx ", k_malloc(40));
	k_printf("%lx ", k_malloc(512));
	k_printf("%lx ", k_malloc(516));
	k_printf("%lx ", k_malloc(780));
	k_printf("%lx ", k_malloc(16));
	k_printf("\n");
#endif

	k_acpi_get_info(rsdp);
	k_smbios_get_info(smbios);

	k_lapic_init();

#if 0
	error = k_hpet_init();
	if (!error) {
		k_pic_uninit();

		k_ioapic_init();
		k_lapic_init();
	} else {
		k_pic_init();
		k_pit_init();
	}

	asm volatile("sti");
#endif

#if 0
#ifdef K_CONFIG_SMP
	k_smp_init();
#endif
#endif

#if 0
	k_loader_init();
	k_initramfs_get_info(initramfs_start, initramfs_length);

	k_irq_mask(0);
	for (int i = 0; i < 100000000; i++)
		asm volatile("nop");
	k_irq_unmask(0);
#endif

	//k_init();
}

