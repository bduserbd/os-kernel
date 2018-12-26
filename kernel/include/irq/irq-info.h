#ifndef K_IRQ_INFO_H
#define K_IRQ_INFO_H

#include "kernel/include/irq/irq.h"

#define K_NUMBER_OF_IRQS	32

#define K_INVALID_IRQ	((unsigned int)~0)

struct k_irq_chip {
	const char *name;

	unsigned int irqs;

	k_error_t (*start)(struct k_irq_chip *);
	void (*reset)(struct k_irq_chip *);

	void (*ack)(struct k_irq_chip *, unsigned int);

	void (*mask)(struct k_irq_chip *, unsigned int);
	void (*unmask)(struct k_irq_chip *, unsigned int);

	unsigned int (*int_to_irq)(struct k_irq_chip *, unsigned int);
	unsigned int (*int_from_irq)(struct k_irq_chip *, unsigned int);

	void *data;

	struct k_irq_chip *next;
};

struct k_irq_action {
	void *device;
	k_irq_handler_t handler;

	struct k_irq_action *next;
};

enum {
	K_IRQ_STATUS_DISABLED,
	K_IRQ_STATUS_ACTIVATED,
};

struct k_irq_descriptor {
	unsigned int irq;
	unsigned int status;
	unsigned int flags;

	struct k_irq_action *actions;
};

extern struct k_irq_descriptor k_irqs[K_NUMBER_OF_IRQS];

k_error_t k_irq_register_chip(struct k_irq_chip *);
k_error_t k_irq_execute_handler(unsigned int);
k_error_t k_irq_execute_handler_custom(unsigned int, void *);
void k_irq_ack(unsigned int);
void k_irq_mask(unsigned int);
void k_irq_unmask(unsigned int);
unsigned int k_irq_from_int(unsigned int);
unsigned int k_irq_to_int(unsigned int);

#endif

