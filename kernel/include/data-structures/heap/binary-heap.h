#ifndef K_BINARY_HEAP
#define K_BINARY_HEAP

enum {
	K_BINARY_HEAP_MIN,
	K_BINARY_HEAP_MAX,
};

struct k_binary_heap {
	int type;

	unsigned int total_elements;
	unsigned int index;

	void **elements;

	int (*compare)(void *, void *);
};

void k_binary_heap_insert(struct k_binary_heap *, void *);

void *k_binary_heap_get_root(struct k_binary_heap *);
void *k_binary_heap_fetch_root(struct k_binary_heap *);

struct k_binary_heap *k_binary_heap_init(int, unsigned int, int (*)(void *, void *));
void k_binary_heap_uninit(struct k_binary_heap *);

#endif

