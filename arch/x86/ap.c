#include "include/ap.h"
#include "include/idt.h"
#include "include/lapic.h"
#include "kernel/include/spinlock.h"
#include "kernel/include/video/print.h"

k_spin_lock_t test;

void k_ap_main(void)
{
	k_idt_load();

	k_spin_lock(&test);

	k_printf("AP CPU: APIC number: %u\n", k_lapic_id());

	k_spin_unlock(&test);

	// TODO: Fix this
	k_lapic_enable();
	k_lapic_spurious_vector_init();
	k_lapic_timer_set_mode(K_LAPIC_LVT_TIMER_MODE_PERIODIC, 0);

	k_schedule();

	while (1) ;
}

