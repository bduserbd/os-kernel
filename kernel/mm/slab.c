#include "include/mm/mm.h"
#include "include/mm/slab.h"
#include "include/mm/buddy.h"
#include "include/video/print.h"

static struct k_cache k_cache_boot = {
	.name = "k_cache",
	.object_size = sizeof(struct k_cache),
};

static struct k_cache *k_cache_cache = NULL;

static struct {
	const char *name;
	unsigned int object_size;
} k_malloc_info[] = {
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
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
};

static struct k_cache *k_malloc_caches[K_MALLOC_MAX_SIZE_LOG2];

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

static void k_cache_init(struct k_cache *cache, unsigned int alignment, unsigned int flags)
{
	int i;
	unsigned int size, s;
	unsigned int free_space, left_over, objects;

	if (cache->object_size * K_SLAB_MIN_OBJECTS > (1 << K_BUDDY_MAX_BLOCK_LOG2))
		return;

	cache->flags = flags;

	if (alignment) {
		if (alignment & (alignment - 1))
			return;

		cache->alignment = alignment;
	} else
		cache->alignment = K_SLAB_MIN_ALIGNMENT;

	if (cache->flags & K_SLAB_FLAGS_L1_ALIGNMENT) {
		unsigned int l1_size = k_cpu_cache_line_size();

		if (cache->alignment < l1_size)
			cache->alignment = l1_size;
	}

	s = K_ALIGN_UP(cache->object_size, cache->alignment) + sizeof(k_cache_free_object_t);

	for (i = K_BUDDY_MIN_BLOCK_LOG2; i <= K_BUDDY_MAX_BLOCK_LOG2; i++) {
		size = 1 << i;

		free_space = size - sizeof(struct k_slab);

		objects = free_space / s;

		if (objects < K_SLAB_MIN_OBJECTS)
			continue;

		cache->objects = objects;
		if (cache->objects > K_SLAB_MAX_OBJECTS)
			cache->objects = K_SLAB_MAX_OBJECTS;

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

struct k_cache *k_cache_create_cache(const char *name, unsigned int object_size,
		unsigned int alignment, unsigned int flags)
{
	struct k_cache *cache;

	cache = k_cache_alloc(k_cache_cache);
	if (!cache)
		return NULL;

	cache->name = name;
	cache->object_size = object_size;
	k_cache_init(cache, alignment, flags);

	return cache;
}

static void k_cache_create_malloc_cache(int index, const char *name, unsigned int object_size)
{
	struct k_cache *cache;

	cache = k_cache_create_cache(name, object_size, object_size, 0x0);
	if (!cache)
		return;

	k_malloc_caches[index] = cache;
}

void k_slab_init(void)
{
	int i;

	k_cache_cache = &k_cache_boot;

	k_cache_init(k_cache_cache, 0x0, 0x0);

	for (i = K_MALLOC_MIN_SIZE_LOG2; i <= K_MALLOC_MAX_SIZE_LOG2; i++)
		k_cache_create_malloc_cache(i, k_malloc_info[i].name,
				k_malloc_info[i].object_size);
}

static int k_malloc_cache_index(k_size_t size)
{
	if (size <= 8)
		return 3;
	else if (size <= 16)
		return 4;
	else if (size <= 32)
		return 5;
	else if (size <= 64)
		return 6;
	else if (size <= 128)
		return 7;
	else if (size <= 256)
		return 8;
	else if (size <= 512)
		return 9;
	else if (size <= 1024)
		return 10;
	else if (size <= 2048)
		return 11;
	else if (size <= 4096)
		return 12;
	else if (size <= 8192)
		return 13;
	else if (size <= 16384)
		return 14;
	else if (size <= 32768)
		return 15;
	else if (size <= 65536)
		return 16;

	return -1;
}

void *k_malloc(k_size_t size)
{

}

void k_free(const void *ptr)
{

}

