#ifndef K_NETWORK_CARD_H
#define K_NETWORK_CARD_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

struct k_network_card {
	char name[32];
	k_uint8_t hw_address[6];

	unsigned long io;

	unsigned int irq;
};

#endif

