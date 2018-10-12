#include "include/mm/mm.h"
#include "include/mm/slab.h"
#include "include/mm/buddy.h"
#include "include/video/print.h"

static const int k_slab_max_objects = (sizeof(k_cache_free_object_t) << 8) - 1;

static struct k_cache k_cache_boot = {
	.name = "k_cache",
	.object_size = sizeof(struct k_cache),
};

static struct {
	const char *name;
	unsigned int size;
} k_malloc_info[] = {
	{ "k_malloc-8", 8 },
	{ "k_malloc-16", 16 },
	{ "k_malloc-32", 32 },
	{ "k_malloc-64", 64 },
	{ "k_malloc-128", 128 },
	{ "k_malloc-256", 256 },
	{ "k_malloc-512", 512 },
	{ "k_malloc-1024", 1024 },
	{ "k_malloc-2048", 2048 },
	{ "k_malloc-4096", 4096 },
	{ "k_malloc-8192", 8192 },
	{ "k_malloc-16384", 16384 },
	{ "k_malloc-32768", 32768 },
	{ "k_malloc-65536", 65536 },
	{ NULL, 0 },
};

static void k_cache_grow(struct k_cache *cache)
{
	unsigned int i;
	struct k_slab *slab;
	void *buf, *memory;

	buf = k_buddy_alloc(1 << cache->blocks_log2);
	if (!buf)
		return;

	if (cache->colour == 0)
		memory = (void *)K_ALIGN_UP((unsigned long)buf + sizeof(struct k_slab) +
				cache->objects * sizeof(k_cache_free_object_t),
				cache->alignment);
	else {
		memory = (void *)K_ALIGN_UP((unsigned long)buf + sizeof(struct k_slab) +
				cache->objects * sizeof(k_cache_free_object_t),
				cache->colour_off) + cache->colour_next * cache->colour_off;

		cache->colour_next++;
		if (cache->colour_next > cache->colour)
			cache->colour_next = 0;
	}

	slab = buf;

	slab->memory = memory;

	slab->active = 0;

	slab->free = 0;
	for (i = 0; i < cache->objects - 1; i++)
		slab->free_objects[i] = i + 1;

	slab->free_objects[i] = K_SLAB_OBJECT_LIST_END;

	slab->next = cache->free_slabs;
	cache->free_slabs = slab;
}

unsigned int __attribute__((weak)) k_cpu_cache_line_size(void)
{
	return 32;
}

static void k_cache_init(struct k_cache *cache, unsigned int flags)
{
	int i;
	unsigned int size, s;
	unsigned int free_space, left_over, objects;

	if (cache->object_size * K_SLAB_MIN_OBJECTS > (1 << K_BUDDY_MAX_BLOCK_LOG2))
		return;

	cache->flags = flags;
	if (cache->flags & K_SLAB_FLAGS_L1_ALIGNMENT)
		cache->alignment = k_cpu_cache_line_size();
	else
		cache->alignment = K_SLAB_MIN_ALIGNMENT;

	s = K_ALIGN_UP(cache->object_size, cache->alignment) +
		sizeof(k_cache_free_object_t);

	for (i = K_BUDDY_MIN_BLOCK_LOG2; i <= K_BUDDY_MAX_BLOCK_LOG2; i++) {
		size = 1 << i;

		free_space = size - sizeof(struct k_slab);

		objects = free_space / s;

		if (objects < K_SLAB_MIN_OBJECTS)
			continue;

		cache->objects = objects;
		if (cache->objects > k_slab_max_objects)
			cache->objects = k_slab_max_objects;

		cache->blocks_log2 = i;

		cache->colour_off = k_cpu_cache_line_size();
		if (cache->colour_off < cache->alignment)
			cache->colour_off = cache->alignment;

		left_over = free_space % s;

		cache->colour = left_over / cache->colour_off;

		cache->colour_next = 0;

		break;
	}

	cache->free_slabs = cache->partial_slabs = cache->full_slabs = NULL;

	k_cache_grow(cache);
}

void *k_cache_alloc_object(struct k_cache *cache, struct k_slab *slab)
{
	unsigned int s;
	void *ptr;

	if (slab->free == K_SLAB_OBJECT_LIST_END)
		return NULL;

	s = K_ALIGN_UP(cache->object_size, cache->alignment);
	ptr = (k_uint8_t *)slab->memory + s * slab->free;

	slab->active++;
	slab->free = slab->free_objects[slab->free];

	return ptr;

}

void *k_cache_alloc(struct k_cache *cache)
{
	struct k_slab *slab;
	void *ptr;

	if (cache->partial_slabs) {
		slab = cache->partial_slabs;

		ptr = k_cache_alloc_object(cache, slab);
		if (!ptr)
			return NULL;

		if (cache->objects == slab->active) {
			slab->next = cache->full_slabs;
			cache->full_slabs = slab;
		}
	} else {
		slab = cache->free_slabs;

		ptr = k_cache_alloc_object(cache, slab);
		if (!ptr)
			return NULL;

		cache->free_slabs = slab->next;
		cache->partial_slabs = slab;
	}

	return ptr;
}

void k_cache_init_malloc_cache(const char *name, unsigned int size)
{
	struct k_cache *cache;

	cache = k_cache_alloc(&k_cache_boot);
	if (!cache)
		return;

	k_printf("%s - %x ", name, cache);
}

void k_slab_init(void)
{
	int i;

	k_cache_init(&k_cache_boot, 0x0);

	for (i = 0; k_malloc_info[i].name; i++)
		k_cache_init_malloc_cache(k_malloc_info[i].name, k_malloc_info[i].size);
}

