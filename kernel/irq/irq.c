#include "include/irq/irq-info.h"
#include "include/mm/mm.h"
#include "include/modules/export-symbol.h"

struct k_irq_chip *k_irq_chip = NULL;

struct k_irq_descriptor k_irqs[K_NUMBER_OF_IRQS] = {
	[0 ... K_NUMBER_OF_IRQS - 1] = {
		.status = K_IRQ_STATUS_DISABLED,
		.flags = 0x0,
		.actions = NULL,
	},
};

k_error_t k_irq_register_chip(struct k_irq_chip *chip)
{
	int i;
	k_error_t error;

	if (!chip)
		return K_ERROR_INVALID_PARAMETER;

	if (!chip->start || !chip->reset || !chip->ack ||
			!chip->mask || !chip->unmask || !chip->int_to_irq)
		return K_ERROR_INVALID_PARAMETER;

	if (chip->irqs > K_NUMBER_OF_IRQS)
		return K_ERROR_INVALID_PARAMETER;

	chip->next = k_irq_chip;
	k_irq_chip = chip;

	for (i = 0; i < chip->irqs; i++)
		k_irqs[i].irq = i;

	k_irq_chip->reset(k_irq_chip);

	error = k_irq_chip->start(k_irq_chip);
	if (error)
		return error;

	return K_ERROR_NONE;
}

static k_error_t k_irq_execute_handler_parameter(unsigned int irq, void *data)
{
	k_error_t error;
	struct k_irq_action *action;
	struct k_irq_descriptor *irq_desc;

	if (irq > k_irq_chip->irqs - 1)
		return K_ERROR_INVALID_PARAMETER;

	irq_desc = &k_irqs[irq];
	if (irq_desc->status != K_IRQ_STATUS_ACTIVATED)
		return K_ERROR_FAILURE;

	for (action = irq_desc->actions; action; action = action->next) {
		if (data)
			error = action->handler(irq, data);
		else
			error = action->handler(irq, action->device);

		if (error)
			if (error != K_ERROR_NONE_IRQ)
				return error;
	}

	return K_ERROR_NONE;
}

k_error_t k_irq_execute_handler(unsigned int irq)
{
	return k_irq_execute_handler_parameter(irq, NULL);
}

k_error_t k_irq_execute_handler_custom(unsigned int irq, void *data)
{
	return k_irq_execute_handler_parameter(irq, data);
}

void k_irq_ack(unsigned int irq)
{
	if (!k_irq_chip)
		return;

	k_irq_chip->ack(k_irq_chip, irq);
}

void k_irq_mask(unsigned int irq)
{
	if (!k_irq_chip)
		return;

	k_irq_chip->mask(k_irq_chip, irq);
}

void k_irq_unmask(unsigned int irq)
{
	if (!k_irq_chip)
		return;

	k_irq_chip->unmask(k_irq_chip, irq);
}

unsigned int k_irq_from_int(unsigned int interrupt)
{
	if (!k_irq_chip)
		return K_INVALID_IRQ;

	return k_irq_chip->int_to_irq(k_irq_chip, interrupt);
}

k_error_t k_irq_request(unsigned int irq, k_irq_handler_t handler,
		unsigned int flags, void *device)
{
	struct k_irq_descriptor *irq_desc;
	struct k_irq_action *action;

	if (!k_irq_chip)
		return K_ERROR_UNINITIALIZED;

	if (irq > k_irq_chip->irqs - 1)
		return K_ERROR_INVALID_PARAMETER;

	irq_desc = &k_irqs[irq];

	if (flags & K_IRQ_FLAGS_SHARED) {
		if (!device)
			return K_ERROR_INVALID_PARAMETER;
	} else {
		if ((irq_desc->status == K_IRQ_STATUS_ACTIVATED) &&
				((irq_desc->flags & K_IRQ_FLAGS_SHARED) == 0))
			return K_ERROR_INVALID_PARAMETER;
	}

	action = k_malloc(sizeof(struct k_irq_action));
	if (!action)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	action->device = device;
	action->handler = handler;

	irq_desc->flags |= flags;

	if (irq_desc->status == K_IRQ_STATUS_DISABLED)
		irq_desc->status = K_IRQ_STATUS_ACTIVATED;

	action->next = irq_desc->actions;
	irq_desc->actions = action;

	return K_ERROR_NONE;
}
K_EXPORT_FUNC(k_irq_request);

void k_irq_free(unsigned int irq, void *device)
{
	if (!k_irq_chip)
		return;
}
K_EXPORT_FUNC(k_irq_free);

