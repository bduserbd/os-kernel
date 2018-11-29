#include "include/elf/elf-loader.h"
#include "include/video/print.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size);

k_error_t k_elf_user_check(Elf(Ehdr) *elf, k_size_t *size)
{
        k_error_t error;

	*size = 0;

        if (elf->e_ident[EI_MAG0] != ELFMAG0 ||
                        elf->e_ident[EI_MAG1] != ELFMAG1 ||
                        elf->e_ident[EI_MAG2] != ELFMAG2 ||
                        elf->e_ident[EI_MAG3] != ELFMAG3 ||
                        elf->e_ident[EI_VERSION] != EV_CURRENT ||
                        elf->e_type != ET_EXEC ||
                        elf->e_version != EV_CURRENT)
                return K_ERROR_INVALID_EXECUTABLE_HEADER;

        *size = elf->e_shoff + elf->e_shnum * elf->e_shentsize;

        error = k_elf_arch_check(elf, *size);
        if (error)
                return error;

        return K_ERROR_NONE;
}

k_error_t k_elf_user_load(Elf(Ehdr) *elf)
{
	int i;
	const Elf(Phdr) *segment;

	segment = (const Elf(Phdr) *)((k_uint8_t *)elf + elf->e_phoff);

	for (i = 0; i < elf->e_phnum; i++) {
		if (segment[i].p_type != PT_LOAD)
			continue;

		k_printf("LOAD");
	}

	return K_ERROR_NONE;
}

