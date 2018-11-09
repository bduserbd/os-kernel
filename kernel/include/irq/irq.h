#ifndef K_IRQ_H
#define K_IRQ_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

#define K_IRQ_FLAGS_SHARED	(1 << 0)

typedef k_error_t (*k_irq_handler_t)(unsigned int, void *);

k_error_t k_irq_request(unsigned int, k_irq_handler_t, unsigned int, void *);
void k_irq_free(unsigned int, void *);

#endif

