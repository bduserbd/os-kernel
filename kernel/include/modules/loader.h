#ifndef K_MODULES_LOADER_H
#define K_MODULES_LOADER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/modules/export.h"

k_error_t k_loader(const k_uint8_t *, k_size_t);

#endif

