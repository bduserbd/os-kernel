#include "include/mm/mm.h"
#include "include/mm/slab.h"

static struct k_cache k_cache_boot = {
	.name = "k_cache",
	.obj_size = sizeof(struct k_cache),
};

void k_slab_init(void)
{

}

