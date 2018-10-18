#include "kernel/include/elf/elf-loader.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size)
{
	if (elf->e_ident[EI_CLASS] != ELFCLASS32 ||
			elf->e_ident[EI_DATA] != ELFDATA2LSB ||
			elf->e_machine != EM_386)
		return K_ERROR_INVALID_MODULE_HEADER;

	return K_ERROR_NONE;
}

