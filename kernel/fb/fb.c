#include "include/fb/fb.h"
#include "include/mm/buddy.h"
#include "include/string.h"

static struct {
	int set;

	struct k_fb_info info;

	struct {
		k_uint32_t x, y;
		k_uint32_t w, h;
	} dirty_area;

	void *double_buffer;
} k_fb;

#define K_FB_INVALID_POS	(k_uint32_t)~0

static void k_fb_reset_dirty_area(void)
{
	k_fb.dirty_area.x = k_fb.dirty_area.y =
	k_fb.dirty_area.w = k_fb.dirty_area.h = K_FB_INVALID_POS;
}

void k_fb_set(struct k_fb_info *fb)
{
	k_memcpy(&k_fb.info, fb, sizeof(struct k_fb_info));

	k_fb_reset_dirty_area();

	k_fb.double_buffer = k_buddy_alloc(k_fb.info.width * k_fb.info.height *
			k_fb.info.bytes_per_pixel);
	if (!k_fb.double_buffer)
		return;

	k_fb.set = true;
}

