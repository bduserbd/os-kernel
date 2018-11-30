#ifndef K_NETWORK_CARD_H
#define K_NETWORK_CARD_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"
#include "kernel/include/network/buffer.h"
#include "kernel/include/network/packet.h"

struct k_network_card;

struct k_network_card_operations {
	k_error_t (*transmit)(struct k_network_card *, struct k_network_buffer *);
};

struct k_network_card {
	k_uint8_t hw_address[6];

	struct k_network_packet *packet;

	struct k_network_card_operations *ops;

	void *data;

	struct k_network_card *next;
};

k_error_t k_network_rx(struct k_network_buffer *);

void k_network_card_register(struct k_network_card *);
void k_network_card_unregister(struct k_network_card *);

#endif

