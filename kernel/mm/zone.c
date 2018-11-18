#include "include/mm/zone.h"
#include "include/mm/mm.h"
#include "include/mm/buddy.h"
#include "include/modules/export-symbol.h"

static k_uint8_t *k_dma_zones = NULL;
struct k_memory_zone *k_memory_zones = NULL;

void k_paging_map_dma(unsigned long, unsigned long, unsigned long);

void *k_p2v(const void *physical)
{
	struct k_memory_zone *zone;
	unsigned long frame, offset;

	frame = (unsigned long)physical >> 12;
	offset = (unsigned long)physical & 0xfff;

	for (zone = k_memory_zones; zone; zone = zone->next)
		if (frame >= zone->start_frame && zone->start_frame + zone->total_frames > frame)
			return (void *)K_OFFSET_FROM(zone->frames[frame - zone->start_frame].virtual,
					offset);

	return NULL;
}
K_EXPORT_FUNC(k_p2v);

unsigned long k_p2v_l(unsigned long physical)
{
	return (unsigned long)k_p2v((const void *)physical);
}
K_EXPORT_FUNC(k_p2v_l);

static inline void k_memory_zone_register(struct k_memory_zone *zone)
{
	zone->next = k_memory_zones;
	k_memory_zones = zone;
}

static struct k_memory_zone *k_memory_zone_alloc_desc(unsigned long start_frame, unsigned total_frames,
		int type)
{
	struct k_memory_zone *zone;

	zone = k_malloc(sizeof(struct k_memory_zone));
	if (!zone)
		return NULL;

	zone->type = type;

	zone->start_frame = start_frame;
	zone->total_frames = total_frames;

	return zone;
}

static k_uint8_t *k_memory_zone_alloc_dma_mapping(void)
{
	k_uint8_t *mapping;
	struct k_memory_zone *zone;

	mapping = k_dma_zones;

	for (zone = k_memory_zones; zone; zone = zone->next)
		if (zone->type == K_MEMORY_ZONE_DMA)
			mapping += (zone->total_frames << 12);

	return mapping;
}

void k_memory_zone_dma_add(unsigned long start_frame, unsigned long total_frames)
{
	unsigned long i;
	k_uint8_t *mapping;
	struct k_memory_zone *zone;

	zone = k_memory_zone_alloc_desc(start_frame, total_frames, K_MEMORY_ZONE_DMA);
	if (!zone)
		return;

	zone->frames = k_malloc(total_frames * sizeof(struct k_frame));
	if (!zone->frames)
		return;

	mapping = k_memory_zone_alloc_dma_mapping();

	k_paging_map_dma((unsigned long)mapping, zone->start_frame << 12,
			zone->total_frames << 12);

	for (i = 0; i < zone->total_frames; i++)
		zone->frames[i].virtual = mapping + (i << 12);

	k_memory_zone_register(zone);
}
K_EXPORT_FUNC(k_memory_zone_dma_add);

void k_memory_zone_init(struct k_frame *k_normal_frames, unsigned long start_frame,
		unsigned long total_frames)
{
	struct k_memory_zone *zone;

	zone = k_memory_zone_alloc_desc(start_frame, total_frames, K_MEMORY_ZONE_NORMAL);
	if (!zone)
		return;

	zone->frames = k_normal_frames;

	k_memory_zone_register(zone);

	k_dma_zones = k_heap + (1 << K_BUDDY_MAX_BLOCK_LOG2);
}

