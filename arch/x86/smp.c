#include "include/smp.h"
#include "include/ap.h"
#include "include/lapic.h"
#include "kernel/include/string.h"
#include "kernel/include/acpi/acpi.h"

void puts(const char *);
void puthex(k_uint32_t);

extern __u8 __k_ap_start[];
extern __u8 __k_ap_end[];

extern __u8 __k_stack_start[];
extern __u8 __k_stack_end[];

static struct k_smp_info {
	k_uint32_t esp[K_CONFIG_CPUS];
} k_smp;

void k_smp_init(void)
{
	int i;
	struct k_acpi_id *id;
	struct k_ap_start_info *info;

	k_uint32_t esp;
	static const k_uint32_t cpu_stack_size = K_CONFIG_STACK_SIZE / K_CONFIG_CPUS;

	if (!k_acpi.found)
		return;

	esp = (k_uint32_t)__k_stack_start - cpu_stack_size;

	k_memcpy((void *)0x8000, __k_ap_start, __k_ap_end - __k_ap_start);

	info = (void *)(0x8000 + 0x180);
	info->entry = (k_uint32_t)k_ap_main;

	id = &k_acpi.ids[0];

	for (i = 0; i < K_CONFIG_CPUS; i++) {
		if (!id[i].valid)
			continue;

		if (id[i].lapic == k_lapic_id()) {
			k_smp.esp[id[i].processor] = (k_uint32_t)__k_stack_start;
		} else {
			k_smp.esp[id[i].processor] = info->esp = esp;
			info->processor = id[i].processor;

			k_lapic_icr_init(id[i].processor);
			k_lapic_icr_start_up(id[i].processor, 0x8000 >> 12);

			esp -= cpu_stack_size;
		}
	}
}

