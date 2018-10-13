#include "include/mm/mm.h"
#include "include/mm/slab.h"
#include "include/video/print.h"

static struct k_cache k_cache_boot = {
	.name = "k_cache",
	.object_size = sizeof(struct k_cache),
};

static struct k_cache *k_cache_cache = NULL;
static struct k_cache *k_cache_management = NULL;

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

static struct k_cache *k_malloc_caches[K_MALLOC_MAX_SIZE_LOG2 + 1];

unsigned int __attribute__((weak)) k_cpu_cache_line_size(void)
{
	return 32;
}

static void k_cache_get_alignment(struct k_cache *cache, unsigned int alignment)
{
	if (alignment) {
		if (alignment & (alignment - 1))
			return;

		cache->alignment = alignment;
	} else
		cache->alignment = K_SLAB_MIN_ALIGNMENT;

	if (cache->flags & K_SLAB_FLAGS_L1_ALIGNMENT) {
		unsigned int l1_line_size = k_cpu_cache_line_size();

		if (cache->alignment < l1_line_size)
			cache->alignment = l1_line_size;
	}
}

static unsigned int k_cache_get_slab_management(struct k_cache *cache)
{
	unsigned int object_space;

	object_space = K_ALIGN_UP(cache->object_size, cache->alignment);

	if (object_space >= ((1 << K_BUDDY_MIN_BLOCK_LOG2) >> K_SLAB_MANAGEMENT_BLOCK_LOG2))
		cache->flags |= K_SLAB_FLAGS_MANAGEMENT_OFF;
	else
		object_space += sizeof(k_cache_free_object_t);

	return object_space;
}

static void k_cache_get_colour(struct k_cache *cache,
		unsigned int free_space, unsigned int object_space)
{
	unsigned int left_over;

	cache->colour_off = k_cpu_cache_line_size();
	if (cache->colour_off < cache->alignment)
		cache->colour_off = cache->alignment;

	left_over = free_space % object_space;

	cache->colour = left_over / cache->colour_off;

	cache->colour_next = 0;
}

static void k_cache_grow(struct k_cache *cache)
{
	unsigned int i;
	struct k_slab *slab;
	void *buf, *memory;

	buf = k_buddy_alloc(1 << cache->blocks_log2);
	if (!buf)
		return;

	if (cache->flags & K_SLAB_FLAGS_MANAGEMENT_OFF) {
		k_printf("$");
	} else {
		unsigned long management_end = (unsigned long)buf + sizeof(struct k_slab) +
			cache->objects * sizeof(k_cache_free_object_t);

		if (cache->colour == 0)
			memory = (void *)K_ALIGN_UP(management_end, cache->alignment);
		else {
			memory = (void *)K_ALIGN_UP(management_end, cache->colour_off) +
				cache->colour_next * cache->colour_off;

			cache->colour_next++;
			if (cache->colour_next > cache->colour)
				cache->colour_next = 0;
		}

		slab = buf;
	}

	slab->memory = memory;

	slab->active = 0;

	slab->free = 0;
	for (i = 0; i < cache->objects - 1; i++)
		slab->free_objects[i] = i + 1;

	slab->free_objects[i] = K_SLAB_OBJECT_LIST_END;

	slab->next = cache->free_slabs;
	cache->free_slabs = slab;
}

static void k_cache_init(struct k_cache *cache, unsigned int alignment, unsigned int flags)
{
	int i;
	unsigned int free_space, object_space;
	unsigned int objects;

	cache->flags = flags;

	k_cache_get_alignment(cache, alignment);

	object_space = k_cache_get_slab_management(cache);

	for (i = K_BUDDY_MIN_BLOCK_LOG2; i <= K_BUDDY_MAX_BLOCK_LOG2; i++) {
		free_space = (1 << i);
		if ((cache->flags & K_SLAB_FLAGS_MANAGEMENT_OFF) == 0)
			free_space -= sizeof(struct k_slab);

		objects = free_space / object_space;

		if (objects < 1)
			continue;

		cache->objects = objects;
		if (cache->objects > K_SLAB_MAX_OBJECTS)
			cache->objects = K_SLAB_MAX_OBJECTS;

		cache->blocks_log2 = i;

		k_cache_get_colour(cache, free_space, object_space);

		break;
	}

	cache->free_slabs = cache->partial_slabs = cache->full_slabs = NULL;

	k_cache_grow(cache);
}

void *k_cache_alloc_object(struct k_cache *cache, struct k_slab *slab)
{
	unsigned int object_space;
	void *ptr;

	if (slab->free == K_SLAB_OBJECT_LIST_END)
		return NULL;

	object_space = K_ALIGN_UP(cache->object_size, cache->alignment);
	ptr = (k_uint8_t *)slab->memory + object_space * slab->free;

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

		k_cache_grow(cache);
	}

	return ptr;
}

struct k_cache *k_cache_create(const char *name, unsigned int object_size,
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

static void k_cache_malloc_create(int index, const char *name, unsigned int object_size)
{
	struct k_cache *cache;

	if ((1 << index) <= K_SLAB_MAX_ALIGNMENT)
		cache = k_cache_create(name, object_size, object_size, 0x0);
	else
		cache = k_cache_create(name, object_size, K_SLAB_MIN_ALIGNMENT, 0x0);

	if (!cache)
		return;

	k_malloc_caches[index] = cache;
}

void k_slab_init(void)
{
	int i;

	k_cache_cache = &k_cache_boot;

	k_cache_init(k_cache_cache, 0x0, 0x0);

	k_cache_management = k_cache_create("k_cache_management", sizeof(struct k_slab) +
			(1 << K_SLAB_MANAGEMENT_BLOCK_LOG2), 0x0, 0x0);
	if (!k_cache_management)
		return;

	for (i = K_MALLOC_MIN_SIZE_LOG2; i <= K_MALLOC_MIN_SIZE_LOG2 + 6; i++)
		k_cache_malloc_create(i, k_malloc_info[i].name, k_malloc_info[i].object_size);
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
	int index;

	index = k_malloc_cache_index(size);
	if (index == -1)
		return NULL;

	if (!k_malloc_caches[index])
		return NULL;

	return k_cache_alloc(k_malloc_caches[index]);
}

void k_free(const void *ptr)
{

}

