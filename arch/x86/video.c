#include "include/video.h"

static unsigned char *k_text_video = NULL;
static k_uint32_t k_text_rows = 0;
static k_uint32_t k_text_cols = 0;

static void k_text_clean(void)
{
	int i, j;

	for (i = 0; i < k_text_rows; i++)
		for (j = 0; j < k_text_cols; j++)
			k_text_video[2 * (k_text_cols * i + j)] = ' ';
}

void k_text_puts(const char *s)
{
	static int x = 0;
	static int y = 0;

#define END_OF_LINE()		\
	y++;			\
	x = 0;			\
				\
	if (y == k_text_rows) {	\
		k_text_clean();	\
		y = 0;		\
	}

	while (*s) {
		if (*s == '\n') {
			END_OF_LINE();
		} else {
			if (x == k_text_cols) {
				END_OF_LINE();
			}

			k_text_video[2 * (k_text_cols * y + x)] = *s;
			x++;
		}

		s++;
	}
}

void k_text_set_info(struct k_fb_info *fb)
{
	k_text_video = (unsigned char *)fb->framebuffer;

	k_text_rows = fb->height;
	k_text_cols = fb->width;

	k_text_clean();
}

