#include "include/network/protocol/address-cache.h"
#include "include/mm/mm.h"
#include "include/string.h"

struct k_address_cache_entry {
	k_ipv4_t ip;
	k_uint8_t mac[K_MAC_LENGTH];

	struct k_address_cache_entry *next;
};

struct k_address_cache {
	struct k_network_card *card;
	struct k_address_cache_entry *entry;

	struct k_address_cache *next;
};

struct k_address_cache *k_address_caches = NULL;

static inline void k_address_cache_entry_add(struct k_address_cache *cache,
		struct k_address_cache_entry *entry)
{
	entry->next = cache->entry;
	cache->entry = entry;
}

static inline void k_address_cache_add(struct k_address_cache *cache)
{
	cache->next = k_address_caches;
	k_address_caches = cache;
}

void k_address_cache_new_entry(struct k_network_card *card, k_ipv4_t ip,
		k_uint8_t mac[K_MAC_LENGTH])
{
	int found = false;
	struct k_address_cache *cache;
	struct k_address_cache_entry *entry;

	for (cache = k_address_caches; cache; cache = cache->next)
		if (cache->card == card) {
			found = true;
			break;
		}

	if (!found)
		return;

	entry = k_malloc(sizeof(struct k_address_cache_entry));
	if (!entry)
		return;

	entry->ip = ip;
	k_memcpy(entry->mac, mac, K_MAC_LENGTH);

	k_address_cache_entry_add(cache, entry);
}

void k_address_cache_resolve(struct k_network_card *card, k_ipv4_t ip,
		k_uint8_t *mac)
{
	int found = false;
	struct k_address_cache *cache;
	struct k_address_cache_entry *entry;

	for (cache = k_address_caches; cache; cache = cache->next)
		if (cache->card == card)
			for (entry = cache->entry; entry; entry = entry->next)
				if (entry->ip == ip) {
					k_memcpy(mac, entry->mac, K_MAC_LENGTH);
					found = true;
					break;
				}

	if (!found)
		return;
}

void k_address_cache_create(struct k_network_card *card)
{
	struct k_address_cache *cache;

	cache = k_malloc(sizeof(struct k_address_cache));
	if (!cache)
		return;

	cache->card = card;
	cache->entry = NULL;

	k_address_cache_add(cache);
}

