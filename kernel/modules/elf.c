#include "include/elf/elf-loader.h"
#include "include/string.h"
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

	return K_ERROR_NONE;
}

void *k_elf_section_by_name(Elf(Ehdr) *elf, const char *name)
{
	int i;
	const char *strings;
	const Elf(Shdr) *section, *strtab;

	section = (const Elf(Shdr) *)((k_uint8_t *)elf + elf->e_shoff);

	strtab = (const Elf(Shdr) *)((k_uint8_t *)elf + elf->e_shoff + elf->e_shstrndx *
			elf->e_shentsize);
	strings = (const char *)elf + strtab->sh_offset;

	for (i = 0; i < elf->e_shnum; i++)
		if (!k_strncmp(strings + section[i].sh_name, name, K_ELF_MAX_SECTION_NAME_LENGTH))
			return (k_uint8_t *)elf + section[i].sh_offset;

	return NULL;
}

k_error_t k_elf_load_image(Elf(Ehdr) *elf, struct k_module *mod)
{
	int i;
	int count;
	k_size_t image_size;
	const Elf(Shdr) *section;

	image_size = 0x0;

	section = (const Elf(Shdr) *)((k_uint8_t *)elf + elf->e_shoff);

	count = 0;

	for (i = 0; i < elf->e_shnum; i++) {
		if ((section[i].sh_flags & SHF_ALLOC) == 0 || !section[i].sh_size)
			continue;

		count++;

		image_size = K_ALIGN_UP(image_size, section[i].sh_addralign) +
				section[i].sh_size;
	}

	return K_ERROR_NONE;
}

