#ifndef K_MODULE_H
#define K_MODULE_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/modules/export.h"

struct k_module {
	char name[K_MODULE_MAX_NAME_LENGTH];

	void (*init)(void);
	void (*uninit)(void);

	struct k_module *next;
};

#endif

