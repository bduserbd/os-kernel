#ifndef K_KERNEL_EXPORT_SYMBOL_H
#define K_KERNEL_EXPORT_SYMBOL_H

#include "kernel/include/types.h"

typedef enum {
	K_EXPORT_TYPE_FUNC,
	K_EXPORT_TYPE_VAR,
} k_export_t;

struct k_export_symbol {
	const char *name;
	k_export_t type;
	void *address;

	struct k_export_symbol *next;
};

extern struct k_export_symbol __k_symtab_start[];
extern struct k_export_symbol __k_symtab_end[];

#define __K_EXPORT_SYMBOL(sym, type)					\
	extern typeof(sym) sym;						\
									\
	static const char __k_strtab_##sym[]				\
	__attribute__((section(".k_strtab"), aligned(1))) = #sym;	\
									\
	static const struct k_export_symbol __k_symtab_##sym		\
	__attribute__((section(".k_symtab"), used))			\
	= { __k_strtab_##sym, type, &sym, NULL }

#define K_EXPORT_FUNC(sym)	\
	__K_EXPORT_SYMBOL(sym, K_EXPORT_TYPE_FUNC)

#define K_EXPORT_VAR(sym)	\
	__K_EXPORT_SYMBOL(sym, K_EXPORT_TYPE_VAR)

#endif

