#include "include/gdt.h"

void k_user_mode_init(void)
{
	k_error_t error;

	error = k_tss_init();
	if (error)
		return;

	k_gdt_init();
}

