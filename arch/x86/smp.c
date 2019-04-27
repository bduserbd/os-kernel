#include "include/smp.h"
#include "include/ap.h"
#include "include/lapic.h"
#include "include/i386/paging.h"
#include "kernel/include/string.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/mm.h"
#include "kernel/include/mm/zone.h"

extern __u8 __k_ap_start[];
extern __u8 __k_ap_end[];

extern __u8 __k_stack_start[];
extern __u8 __k_stack_end[];

void k_paging_map_ap_start(unsigned long);

void k_smp_init(void)
{
	int i;
	struct k_acpi_id *id;
	struct k_ap_start_info *info;
	unsigned long sp;
	static const int cpu_stack_size = K_CONFIG_STACK_SIZE / K_CONFIG_CPUS;

	if (!k_acpi.found)
		return;

	sp = (unsigned long)__k_stack_start - cpu_stack_size;

	k_paging_map_ap_start(K_AP_START_ADDRESS);

	k_memcpy((void *)K_AP_START_ADDRESS, __k_ap_start, __k_ap_end - __k_ap_start);

	info = (void *)K_AP_START_INFO;
	info->page_table_physical = (unsigned long)k_page_table - K_IMAGE_BASE;
	info->entry = (unsigned long)k_ap_main;

	id = &k_acpi.ids[0];

	for (i = 0; i < K_CONFIG_CPUS; i++) {
		if (!id[i].valid)
			continue;

		if (id[i].lapic != k_lapic_id()) {
			/* TODO: Check that each processor gets its own stack. */
			info->sp = sp;

			k_lapic_icr_init(id[i].processor);
			k_lapic_icr_start_up(id[i].processor, K_AP_START_ADDRESS >> 12);

			sp -= cpu_stack_size;
		}
	}
}

