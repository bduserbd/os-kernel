#include "include/gdt.h"
#include "include/eflags.h"

static void k_user_mode_switch(void (*callback)(void *), void *stack)
{
	asm volatile(
		"mov	%0, %%ax	\n"
		"mov	%%ax, %%ds	\n"
		"mov	%%ax, %%es	\n"
		"mov	%%ax, %%fs	\n"
		"mov	%%ax, %%gs	\n"
		"push	%0		\n"
		"push	%1		\n"
		"push	%2		\n"
		"push	%3		\n"
		"push	%4		\n"
		"iret"
		: :
		"i" (K_GDT_USER_DATA_SELECTOR | 3),
		"m" (stack),
		"i" (K_EFLAGS_FIXED | K_EFLAGS_IF),
		"i" (K_GDT_USER_CODE_SELECTOR | 3),
		"m" (callback)
	);

	for (;;) ;
}

void k_user_mode_arch_init(void (*callback)(void *), void *stack)
{
	k_error_t error;

	error = k_tss_init();
	if (error)
		return;

	k_gdt_init();

	k_user_mode_switch(callback, stack);
}

