#ifndef K_LATE_IRQ_H
#define K_LATE_IRQ_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

enum {
	K_LATE_IRQ_NETWORK_RX,
	K_LATE_IRQ_COUNT,
};

typedef k_error_t (*k_late_irq_handler_t)(void);

k_error_t k_late_irq_request(unsigned int, k_late_irq_handler_t);
k_error_t k_late_irq_raise(unsigned int);

k_error_t k_late_irq_task(void *);

#endif

