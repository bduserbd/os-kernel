#include "include/ap.h"
#include "include/idt.h"
#include "kernel/include/spinlock.h"
#include "kernel/include/video/print.h"

k_spin_lock_t test;

void k_ap_main(k_uint32_t processor)
{
	k_idt_load();

	k_spin_lock(&test);

	k_printf("AP CPU: APIC number: %u\n", processor);

	while (1) ;
}

