#include "include/irq/late-irq.h"

static k_uint32_t k_late_irq_bits = 0x0;
static k_late_irq_handler_t k_late_irq_handlers[K_LATE_IRQ_COUNT] = { 0 };

k_error_t k_late_irq_request(unsigned int late_irq, k_late_irq_handler_t handler)
{
	if (late_irq > K_LATE_IRQ_COUNT)
		return K_ERROR_INVALID_PARAMETER;

	if (k_late_irq_handlers[late_irq])
		return K_ERROR_ALREADY_INITIALIZED;

	k_late_irq_handlers[late_irq] = handler;

	return K_ERROR_NONE;
}

k_error_t k_late_irq_raise(unsigned int late_irq)
{
	if (!k_late_irq_handlers[late_irq])
		return K_ERROR_UNINITIALIZED;

	k_late_irq_bits |= (1 << late_irq);

	return K_ERROR_NONE;
}

k_error_t k_late_irq_task(void *parameter)
{
	int i;

	while (1) {
		if (!k_late_irq_bits)
			continue;

		for (i = 0; i < 32; i++)
			if (k_late_irq_bits & (1 << i)) {
				k_late_irq_handlers[i]();

				k_late_irq_bits &= ~(1 << i);
			}
	}

	return K_ERROR_FATAL;
}

