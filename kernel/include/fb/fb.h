#ifndef K_FRAMEBUFFER_H
#define K_FRAMEBUFFER_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

typedef k_uint32_t	k_fb_color_t;

struct k_fb_color_mask {
	k_uint8_t position;
	k_uint8_t mask;
};

#define K_FB_INVALID_POS	(k_uint32_t)~0

struct k_fb_info {
	/* Screen size. */
	k_uint32_t width;
	k_uint32_t height;
	k_uint32_t pitch;

	/* Video memory area. */
	k_addr_t framebuffer;

	/* Pixel definitions. */
	k_uint8_t bits_per_pixel;
	k_uint8_t bytes_per_pixel;

	/* Color definitions. */
	struct k_fb_color_mask red;
	struct k_fb_color_mask green;
	struct k_fb_color_mask blue;
	struct k_fb_color_mask reserved;
};

void k_fb_set_info(struct k_fb_info *);
k_fb_color_t k_fb_prepare_color(k_uint8_t, k_uint8_t, k_uint8_t, k_uint8_t);
void k_fb_blit_glyph(char, k_uint32_t, k_uint32_t);
void k_fb_update_framebuffer(void);
k_uint32_t k_fb_width(void);
k_uint32_t k_fb_height(void);

#endif

