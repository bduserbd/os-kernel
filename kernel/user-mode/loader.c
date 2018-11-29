#include "include/user-mode/user-mode.h"
#include "include/elf/elf-loader.h"
#include "include/video/print.h"

extern __u8 __k_user_start[];

k_error_t k_elf_user_check(Elf(Ehdr) *, k_size_t *);
k_error_t k_elf_user_load(Elf(Ehdr) *);

void k_user_mode_init(void)
{
	k_size_t size;
	k_error_t error;
	Elf(Ehdr) *elf;

	elf = (Elf(Ehdr) *)__k_user_start;

	error = k_elf_user_check(elf, &size);
	if (error)
		return;	

	error = k_elf_user_load(elf);
	if (error)
		return;
}

