#include "include/fb/fb.h"
#include "include/fb/font.h"
#include "include/mm/buddy.h"
#include "include/mm/mm.h"
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

k_uint32_t k_fb_width(void)
{
	if (k_fb.set)
		return k_fb.info.width;
	else
		return K_FB_INVALID_POS;
}

k_uint32_t k_fb_height(void)
{
	if (k_fb.set)
		return k_fb.info.height;
	else
		return K_FB_INVALID_POS;
}

k_fb_color_t k_fb_prepare_color(k_uint8_t r, k_uint8_t g, k_uint8_t b, k_uint8_t reserved)
{
	if (!k_fb.set)
		return 0;

	return	((r & k_fb.info.red.mask) << k_fb.info.red.position) |
		((g & k_fb.info.green.mask) << k_fb.info.green.position) |
		((b & k_fb.info.blue.mask) << k_fb.info.blue.position) |
		((reserved & k_fb.info.reserved.mask) << k_fb.info.reserved.position);
}

static void k_fb_reset_dirty_area(void)
{
	k_fb.dirty_area.x = k_fb.dirty_area.y =
	k_fb.dirty_area.w = k_fb.dirty_area.h = K_FB_INVALID_POS;
}

static void k_fb_update_dirty_area(k_uint32_t x, k_uint32_t y, k_uint32_t w, k_uint32_t h)
{
	k_uint32_t dirty_x, dirty_y;
	k_uint32_t dirty_w, dirty_h;

	dirty_x = k_fb.dirty_area.x;
	dirty_y = k_fb.dirty_area.y;
	dirty_w = k_fb.dirty_area.w;
	dirty_h = k_fb.dirty_area.h;

	if (x < dirty_x || dirty_x == K_FB_INVALID_POS)
		k_fb.dirty_area.x = x;

	if (y < dirty_y || dirty_y == K_FB_INVALID_POS)
		k_fb.dirty_area.y = y;

	if (dirty_w == K_FB_INVALID_POS)
		k_fb.dirty_area.w = w;
	else {
		if (x < dirty_x) {
			if ((x + w) < (dirty_x + dirty_w))
				k_fb.dirty_area.w = dirty_x + dirty_w - x;
			else
				k_fb.dirty_area.w = w;
		} else {
			if (x + w > dirty_x + dirty_w)
				k_fb.dirty_area.w = x + w - dirty_x;
		}
	}

	if (dirty_h == K_FB_INVALID_POS)
		k_fb.dirty_area.h = h;
	else {
		if (y < dirty_y) {
			if (y + h < dirty_y + dirty_h)
				k_fb.dirty_area.h = dirty_y + dirty_h - y;
			else
				k_fb.dirty_area.h = h;
		} else {
			if (y + h > dirty_y + dirty_h)
				k_fb.dirty_area.h = y + h - dirty_y;
		}
	}
}

static inline k_uint8_t *k_fb_double_buffer_start(k_uint32_t x, k_uint32_t y)
{
	return (k_uint8_t *)k_fb.double_buffer + y * k_fb.info.pitch + x * k_fb.info.bytes_per_pixel;
}

static inline k_uint8_t *k_fb_framebuffer_start(k_uint32_t x, k_uint32_t y)
{
	return (k_uint8_t *)k_fb.info.framebuffer + y * k_fb.info.pitch + x * k_fb.info.bytes_per_pixel;
}

void k_fb_blit_glyph(char c, k_uint32_t x, k_uint32_t y)
{
	int i, j;
	k_uint8_t *glyph;
	k_uint8_t *dst;
	k_fb_color_t white, black;

	if (!k_fb.set)
		return;

	if (c < 0x20)
		return;

	k_fb_update_dirty_area(x, y, K_FONT_CHARACTER_WIDTH, K_FONT_CHARACTER_HEIGHT);

	glyph = (k_uint8_t *)&k_font[(int)c];

	dst = k_fb_double_buffer_start(x, y);

	white = k_fb_prepare_color(0xff, 0xff, 0xff, 0xff);
	black = k_fb_prepare_color(0x0, 0x0, 0x0, 0x0);

	switch (k_fb.info.bytes_per_pixel) {
	case 3:
		for (i = 0; i < K_FONT_CHARACTER_HEIGHT; i++) {
			int w = K_FONT_CHARACTER_WIDTH - 1;
			for (j = w; j >= 0; j--) {
				k_fb_color_t c;

				if (glyph[i] & (1 << j))
					c = white;
				else
					c = black;

				((k_uint8_t *)dst + (w - j) * 3)[0] = c & 0xff;
				((k_uint8_t *)dst + (w - j) * 3)[1] = (c >> 8) & 0xff;
				((k_uint8_t *)dst + (w - j) * 3)[2] = (c >> 16) & 0xff;
			}

			dst += k_fb.info.pitch;
		}

		break;

	case 4:
		for (i = 0; i < K_FONT_CHARACTER_HEIGHT; i++) {
			k_uint64_t fg = ((k_uint64_t)white << 32) | white;
			k_uint64_t bg = ((k_uint64_t)black << 32) | black;

			k_uint64_t mask1 = k_font_lookup_table4[(glyph[i] >> 6) & 0x3];
			*(k_uint64_t *)dst = (fg & mask1) | (bg & ~mask1);

			k_uint64_t mask2 = k_font_lookup_table4[(glyph[i] >> 4) & 0x3];
			*(k_uint64_t *)(dst + 8) = (fg & mask2) | (bg & ~mask2);

			k_uint64_t mask3 = k_font_lookup_table4[(glyph[i] >> 2) & 0x3];
			*(k_uint64_t *)(dst + 16) = (fg & mask3) | (bg & ~mask3);

			k_uint64_t mask4 = k_font_lookup_table4[glyph[i] & 0x3];
			*(k_uint64_t *)(dst + 24) = (fg & mask4) | (bg & ~mask4);

			dst += k_fb.info.pitch;
		}

		break;

	default:
		break;
	}
}

void k_fb_update_framebuffer(void)
{
	int i;
	k_uint8_t *src, *dst;
	k_uint32_t x, y, w, h;

	if (!k_fb.set)
		return;

	x = k_fb.dirty_area.x;
	y = k_fb.dirty_area.y;
	w = k_fb.dirty_area.w;
	h = k_fb.dirty_area.h;

	if (x == K_FB_INVALID_POS || y == K_FB_INVALID_POS ||
			w == K_FB_INVALID_POS || h == K_FB_INVALID_POS)
		return;

	src = k_fb_double_buffer_start(x, y);
	dst = k_fb_framebuffer_start(x, y);

	for (i = 0; i < h; i++) {
		k_memcpy(dst, src, w * k_fb.info.bytes_per_pixel);
		src += k_fb.info.pitch;
		dst += k_fb.info.pitch;
	}

	k_fb_reset_dirty_area();
}

void k_fb_set_info(struct k_fb_info *fb)
{
	k_memcpy(&k_fb.info, fb, sizeof(struct k_fb_info));
	k_fb.info.framebuffer = k_p2v_l(k_fb.info.framebuffer);

	k_fb_reset_dirty_area();

	k_fb.double_buffer = k_buddy_alloc(k_fb.info.width * k_fb.info.height *
			k_fb.info.bytes_per_pixel);
	if (!k_fb.double_buffer)
		return;

	k_fb.set = true;

#if 0
	k_fb_blit_glyph('O', 0, 0);
	k_fb_blit_glyph('k', 8, 0);

	k_fb_update_framebuffer();
#endif
}

