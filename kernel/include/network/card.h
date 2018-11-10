#ifndef K_NETWORK_CARD_H
#define K_NETWORK_CARD_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/network/buffer.h"

struct k_network_card;

struct k_network_card_operations {
	k_error_t (*transmit)(struct k_network_card *, struct k_network_buffer *);
};

struct k_network_card {
	struct k_network_card_operations *ops;

	void *data;

	struct k_network_card *next;
};

void k_network_card_register(struct k_network_card *);
void k_network_card_unregister(struct k_network_card *);

#endif

