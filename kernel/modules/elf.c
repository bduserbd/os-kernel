#include "include/elf/elf-loader.h"
#include "include/video/print.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size);

k_error_t k_elf_check(Elf(Ehdr) *elf, k_size_t size)
{
	k_error_t error;

	if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
			elf->e_ident[EI_MAG1] != ELFMAG1 ||
			elf->e_ident[EI_MAG2] != ELFMAG2 ||
			elf->e_ident[EI_MAG3] != ELFMAG3 ||
			elf->e_ident[EI_VERSION] != EV_CURRENT ||
			elf->e_type != ET_REL ||
			elf->e_version != EV_CURRENT)
		return K_ERROR_INVALID_MODULE_HEADER;

	error = k_elf_arch_check(elf, size);
	if (error)
		return error;

	if (size != elf->e_shoff + elf->e_shnum * elf->e_shentsize)
		return K_ERROR_INVALID_MODULE_SIZE;

	k_printf("Elf32\n");

	return K_ERROR_NONE;
}

