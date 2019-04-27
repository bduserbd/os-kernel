#include "include/init.h"
#include "include/cpu.h"
#include "include/lapic.h"
#include "include/ioapic.h"
#include "include/tsc.h"
#include "include/hpet.h"
#include "include/smbios.h"
#include "include/8259a.h"
#include "include/8253.h"
#include "include/video.h"
#include "include/paging.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/irq/irq-info.h"
#include "kernel/include/video/print.h"

void k_init(void);

void k_x86_init(void *smbios, void *rsdp)
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

	k_acpi_get_info(rsdp);
	k_smbios_get_info(smbios);

	k_pit_init();
	k_tsc_init();

	k_pic_uninit();

	error = k_lapic_init();
	if (!error) {
		error = k_ioapic_init();
		if (error)
			k_pic_init();
		else
			k_hpet_init();
	} else
		k_pic_init();

	asm volatile("sti");

#ifdef K_CONFIG_SMP
	k_smp_init();
#endif

#if 0
	k_irq_mask(0);
	k_irq_unmask(0);
#endif
	for (int i = 0; i < 100000000; i++)
		asm volatile("nop");

	k_init();
}

