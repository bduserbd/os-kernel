#include "include/elf/elf-loader.h"
#include "include/modules/module.h"
#include "include/mm/mm.h"
#include "include/mm/buddy.h"
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
	int i, j;
	int count;
	const Elf(Shdr) *section;
	k_size_t image_size;
	k_uint8_t *image_ptr;
	k_error_t error;

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

	mod->segments = k_malloc(count * sizeof(struct k_segment));
	if (!mod->segments) {
		error = K_ERROR_MEMORY_ALLOCATION_FAILED;
		goto _exit;
	}

	image_ptr = k_buddy_alloc(image_size);
	if (!image_ptr) {
		error = K_ERROR_MEMORY_ALLOCATION_FAILED;
		goto _exit;
	}

	for (i = 0, j = 0; i < elf->e_shnum; i++) {
		if ((section[i].sh_flags & SHF_ALLOC) == 0 || !section[i].sh_size)
			continue;

		image_ptr = (k_uint8_t *)K_ALIGN_UP((unsigned long)image_ptr,
				section[i].sh_addralign);

		if (section[i].sh_type == SHT_PROGBITS)
			k_memcpy(image_ptr, (k_uint8_t *)elf + section[i].sh_offset,
					section[i].sh_size);
		else if (section[i].sh_type == SHT_NOBITS)
			k_memcpy(image_ptr, 0, section[i].sh_size);

		mod->segments[j].ptr = image_ptr;
		mod->segments[j].size = section[i].sh_size;

		mod->segments[j].index = i;
		j++;

		image_ptr += section[i].sh_size;
	}

	return K_ERROR_NONE;

_exit:
	if (mod->segments)
		k_free(mod->segments);

	return error;
}

static void *k_elf_section_by_index(struct k_module *mod, Elf(Section) index)
{
	int i;

	for (i = 0; i < mod->count; i++)
		if (mod->segments[i].index == index)
			return mod->segments[i].ptr;

	return NULL;
}

static k_error_t k_elf_resolve_symbol(Elf(Sym) *symbol, const char *name,
		struct k_module *mod)
{
	if (name[0])
		k_printf("%s ", name);

	switch (ELF_ST_TYPE(symbol->st_info)) {
	case STT_FUNC:
		symbol->st_value += (unsigned long)k_elf_section_by_index(mod, symbol->st_shndx);

		if (!k_strcmp(name, "k_module_init"))
			mod->init = (void (*)(void))symbol->st_value;
		else if (!k_strcmp(name, "k_module_uninit"))
			mod->uninit = (void (*)(void))symbol->st_value;

		break;

	case STT_NOTYPE:
	case STT_OBJECT:
		if (symbol->st_name && symbol->st_shndx == SHN_UNDEF) {

		} else
			symbol->st_value += (unsigned long)k_elf_section_by_index(mod,
					symbol->st_shndx);

		break;

	case STT_SECTION:
		symbol->st_value += (unsigned long)k_elf_section_by_index(mod, symbol->st_shndx);
		break;

	case STT_FILE:
		symbol->st_value = 0;
		break;

	default:
		return K_ERROR_INVALID_MODULE_ELF;
	}

	return K_ERROR_NONE;
}

k_error_t k_elf_load_symbols(Elf(Ehdr) *elf, struct k_module *mod)
{
	int i, j;
	const Elf(Shdr) *section, *strtab;
	Elf(Sym) *symbol;
	const char *strings;
	k_error_t error;

	section = (const Elf(Shdr) *)((k_uint8_t *)elf + elf->e_shoff);

	for (i = 0; i < elf->e_shnum; i++) {
		if (section[i].sh_type != SHT_SYMTAB)
			continue;

		symbol = (Elf(Sym) *)((k_uint8_t *)elf + section[i].sh_offset);
		strtab = (const Elf(Shdr) *)((k_uint8_t *)elf + elf->e_shoff + section[i].sh_link *
				elf->e_shentsize);
		strings = (const char *)elf + strtab->sh_offset;

		for (j = 0; j < section[i].sh_size / section[i].sh_entsize; j++) {
			error = k_elf_resolve_symbol(&symbol[j], strings + symbol[j].st_name, mod);
			if (error)
				goto _exit;
		}
	}

	error = K_ERROR_NONE;

_exit:
	return error;
}

