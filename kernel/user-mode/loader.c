#include "include/user-mode/user-mode.h"
#include "include/elf/elf-loader.h"
#include "include/video/print.h"

extern __u8 __k_user_start[];

k_error_t k_elf_user_check(Elf(Ehdr) *, k_size_t *);
k_error_t k_elf_user_load(Elf(Ehdr) *, void **);

void k_user_mode_arch_init(void (*)(void *), void *);

void k_user_mode_init(void)
{
	k_size_t size;
	k_error_t error;
	void *stack;
	Elf(Ehdr) *elf;

	elf = (Elf(Ehdr) *)__k_user_start;

	error = k_elf_user_check(elf, &size);
	if (error)
		return;	

	error = k_elf_user_load(elf, &stack);
	if (error)
		return;

	k_user_mode_arch_init((void (*)(void *))elf->e_entry, stack);
}

