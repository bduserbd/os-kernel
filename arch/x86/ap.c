#include "include/ap.h"
#include "kernel/include/video/print.h"

void k_ap_main(k_uint32_t processor)
{
#if 0
	k_puts("AP CPU:APIC number: ");
	k_puthex(processor);
	k_puts(": ");
	k_puthex(*(volatile k_uint32_t *)0xfee00020 >> 24);
#endif
}

