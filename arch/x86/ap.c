#include "include/ap.h"
#include "kernel/include/video/print.h"

void k_ap_main(k_uint32_t processor)
{
	k_printf("AP CPU: APIC number: %u:%u\n", processor,
		*(volatile k_uint32_t *)0xfee00020 >> 24);
}

