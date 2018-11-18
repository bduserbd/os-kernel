#ifndef K_ADDRESS_CACHE_H
#define K_ADDRESS_CACHE_H

#include "kernel/include/network/card.h"
#include "kernel/include/network/protocol/ipv4.h"
#include "kernel/include/network/protocol/ethernet.h"

void k_address_cache_create(struct k_network_card *card);
void k_address_cache_new_entry(struct k_network_card *, k_ipv4_t,
		k_uint8_t [K_MAC_LENGTH]);

void k_address_cache_resolve(struct k_network_card *card, k_ipv4_t, k_uint8_t *);

#endif

