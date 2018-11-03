#include "include/mm/zone.h"
#include "include/mm/mm.h"

struct k_memory_zone *k_memory_zones = NULL;

void k_memory_zone_init(struct k_frame *k_normal_frames, unsigned long start_frame,
		unsigned long total_frames)
{
	struct k_memory_zone *zone;

	zone = k_malloc(sizeof(struct k_memory_zone));
	if (!zone)
		return;

	zone->type = K_MEMORY_ZONE_NORMAL;

	zone->start_frame = start_frame;
	zone->total_frames = total_frames;

	zone->frames = k_normal_frames;

	zone->next = k_memory_zones;
	k_memory_zones = zone;
}

