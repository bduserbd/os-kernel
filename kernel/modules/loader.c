#include "include/modules/loader.h"
#include "include/modules/module.h"
#include "include/elf/elf-loader.h"
#include "include/mm/mm.h"
#include "include/string.h"
#include "include/video/print.h"

static struct k_module *k_mods = NULL;

static int k_loader_module_exists(const char *name)
{
	struct k_module *mod;

	for (mod = k_mods; mod; mod = mod->next)
		if (!k_strncmp(mod->name, name, K_MODULE_MAX_NAME_LENGTH))
			return true;

	return false;
}

k_error_t k_loader(const k_uint8_t *buf, k_size_t size)
{
	char *mod_name;
	Elf(Ehdr) *elf;
	struct k_module *mod;
	k_error_t error;

	if (!buf || !size) {
		error = K_ERROR_INVALID_PARAMETER;
		goto _exit;
	}

	elf = (Elf(Ehdr) *)buf;

	error = k_elf_check(elf, size);
	if (error)
		goto _exit;

	mod_name = k_elf_section_by_name(elf, ".k_mod_name");
	if (!mod_name) {
		error = K_ERROR_INVALID_MODULE_INFO;
		goto _exit;
	}

	if (k_loader_module_exists(mod_name)) {
		error = K_ERROR_MODULE_ALREADY_EXISTS;
		goto _exit;
	}

	mod = k_malloc(sizeof(struct k_module));
	if (!mod) {
		error = K_ERROR_MEMORY_ALLOCATION_FAILED;
		goto _exit;
	}

	k_strncpy(mod->name, mod_name, K_MODULE_MAX_NAME_LENGTH);

	mod->init = NULL;
	mod->uninit = NULL;

	mod->next = NULL;

	error = k_elf_load_image(elf, mod);
	if (error)
		goto _exit;

	error = k_elf_load_symbols(elf, mod);
	if (error)
		goto _exit;

	error = k_elf_relocate_symbols(elf, mod);
	if (error)
		goto _exit;

	if (!mod->init || !mod->uninit) {
		error = K_ERROR_INVALID_MODULE_ELF;
		goto _exit;
	} else
		mod->init();

	k_printf("%s", mod_name);

	return K_ERROR_NONE;

_exit:
	if (mod)
		k_free(mod);

	return error;
}

#define K_SYMBOL_TABLE_ENTRIES	31

struct k_export_symbol *k_symbol_table[K_SYMBOL_TABLE_ENTRIES];

static int k_loader_symbol_hash(const char *str)
{
	k_uint8_t c;
	unsigned long hash = 5381;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash % K_SYMBOL_TABLE_ENTRIES;
}

struct k_export_symbol *k_loader_get_symbol(const char *name)
{
	int entry;
	struct k_export_symbol *symbol;

	entry = k_loader_symbol_hash(name);

	for (symbol = k_symbol_table[entry]; symbol; symbol = symbol->next)
		if (!k_strcmp(symbol->name, name))
			return symbol;

	return NULL;
}

void k_loader_init(void)
{
	int i;
	int entry;

	for (i = 0; i < __k_symtab_end - __k_symtab_start; i++) {
		entry = k_loader_symbol_hash(__k_symtab_start[i].name);

		__k_symtab_start[i].next = k_symbol_table[entry];
		k_symbol_table[entry] = &__k_symtab_start[i];
	}
}

