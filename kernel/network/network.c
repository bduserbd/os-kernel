#include "include/network/card.h"
#include "include/modules/export-symbol.h"

struct k_network_card *k_network_cards = NULL;

void k_ipv4_init(void);

void k_network_init(void)
{
	k_ipv4_init();
}

void k_network_card_register(struct k_network_card *card)
{
	card->next = k_network_cards;
	k_network_cards = card;
}
K_EXPORT_FUNC(k_network_card_register);

void k_network_card_unregister(struct k_network_card *card)
{

}
K_EXPORT_FUNC(k_network_card_unregister);

