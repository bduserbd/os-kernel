#ifndef K_NETWORK_H
#define K_NETWORK_H

#include "kernel/include/network/card.h"
#include "kernel/include/network/buffer.h"

extern struct k_network_card *k_network_cards;

void k_network_init(void);

#endif

