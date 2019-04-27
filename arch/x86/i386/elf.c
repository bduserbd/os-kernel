#include "kernel/include/elf/elf-loader.h"

k_error_t k_elf_arch_check(Elf(Ehdr) *elf, k_size_t size)
{
	if (elf->e_ident[EI_CLASS] != ELFCLASS32 ||
			elf->e_ident[EI_DATA] != ELFDATA2LSB ||
			elf->e_machine != EM_386)
		return K_ERROR_INVALID_MODULE_HEADER;

	return K_ERROR_NONE;
}

k_error_t k_elf_arch_relocate_section(k_uint8_t *applied_section,
		const Elf(Rel) *rel, const Elf(Sym) *symbol)
{
	Elf(Word) *rel32;

	rel32 = (Elf(Word) *)(applied_section + rel->r_offset);

	switch (ELF_R_TYPE(rel->r_info)) {
	case R_386_32:
		*rel32 += symbol[ELF_R_SYM(rel->r_info)].st_value;
		break;

	case R_386_PC32:
		*rel32 += symbol[ELF_R_SYM(rel->r_info)].st_value - (unsigned long)rel32;
		break;

	default:
		break;
	}

	return K_ERROR_NONE;
}

k_error_t k_elf_arch_relocate_addend_section(k_uint8_t *applied_section,
		const Elf(Rel) *rel, const Elf(Sym) *symbol)
{
	return K_ERROR_NOT_IMPLEMENTED;
}

