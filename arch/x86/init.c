#include "include/cpu.h"
#include "include/mp.h"
#include "include/lapic.h"
#include "include/smbios.h"
#include "include/video.h"
#include "kernel/include/acpi/acpi.h"
#include "kernel/include/mm/buddy.h"

#ifdef K_CONFIG_SMP
#include "include/smp.h"
#endif

void k_x86_init(k_uint32_t heap)
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

	void *a;

	a = k_buddy_alloc(0x500); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x6000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x500); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x70000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x500); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x800000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x500); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x70000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x6000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x6000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
	a = k_buddy_alloc(0x70000); k_puthex((k_uint32_t)a); k_buddy_free(a); 
}

