#ifndef K_MODULES_LOADER_H
#define K_MODULES_LOADER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/modules/export.h"
#include "kernel/include/modules/export-symbol.h"

void k_loader_init(void);
k_error_t k_loader(const k_uint8_t *, k_size_t);
struct k_export_symbol *k_loader_get_symbol(const char *);

#endif

