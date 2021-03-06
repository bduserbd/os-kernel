#include "include/fb/shell.h"
#include "include/mm/buddy.h"
#include "include/string.h"

struct k_shell_line {
	k_uint32_t line;
	k_uint32_t column;
};

struct k_shell_content {
	struct k_shell_line updated_line;
	struct k_shell_line current_line;
	char *vector;
};

static struct {
	k_uint32_t x, y;
	k_uint32_t w, h;

	k_uint32_t rows, cols;

	int scroll;

	struct k_shell_content page;
} k_shell;

static inline k_uint32_t k_shell_position(struct k_shell_line *line)
{
	return line->line * k_shell.cols + line->column;
}

void k_shell_reset_page_vector(struct k_shell_content *page,
		k_uint32_t rows, k_uint32_t cols)
{
	k_memset(page->vector, 0, rows * cols);

	page->updated_line.line = page->updated_line.column = 0;
	page->current_line.line = page->current_line.column = 0;
}

k_error_t k_shell_init_page_vector(struct k_shell_content *page,
		k_uint32_t rows, k_uint32_t cols)
{
	page->vector = k_buddy_alloc(rows * cols);
	if (!page->vector)
		return K_ERROR_MEMORY_ALLOCATION_FAILED;

	k_shell_reset_page_vector(page, rows, cols);

	return K_ERROR_NONE;
}

static void k_shell_update_pages(const char *s)
{
	int i;
	struct k_shell_content *page;

	page = &k_shell.page;

	for (i = 0; s[i]; i++) {
		if (page->current_line.column + 1 < k_shell.cols && s[i] != '\n') {
			page->vector[k_shell_position(&page->current_line)] = s[i];
			page->current_line.column++;
		} else {
			page->current_line.line++;
			page->current_line.column = 0;

			if (page->current_line.line == k_shell.rows) {
				k_shell_reset_page_vector(page, k_shell.rows, k_shell.cols);
				k_shell.scroll = 1;
			}
		}
	}
}

void k_shell_scroll(void)
{
	struct k_shell_line a, b;
	k_uint32_t pos, end_pos;

	a.column = 0;
	a.line = 0;

	b.column = k_shell.cols - 1;
	b.line = k_shell.rows - 1;

	pos = k_shell_position(&a);
	end_pos = k_shell_position(&b);

	while (pos < end_pos) {
		k_fb_blit_glyph(' ',
			k_shell.x + (pos % k_shell.cols) * K_FONT_CHARACTER_WIDTH,
			k_shell.y + (pos / k_shell.cols) * K_FONT_CHARACTER_HEIGHT);

		pos++;
	}
}

void k_shell_flush_page_buffers(void)
{
	struct k_shell_content *page;
	k_uint32_t pos, end_pos;

	if (k_shell.scroll) {
		k_shell_scroll();
		k_shell.scroll = 0;
	}

	page = &k_shell.page;

	pos = k_shell_position(&page->updated_line);
	end_pos = k_shell_position(&page->current_line);

	while (pos < end_pos) {
		k_fb_blit_glyph(page->vector[pos],
				k_shell.x + (pos % k_shell.cols) * K_FONT_CHARACTER_WIDTH,
				k_shell.y + (pos / k_shell.cols) * K_FONT_CHARACTER_HEIGHT);

		pos++;

		if (!page->vector[pos])
			pos += k_shell.cols - pos % k_shell.cols;
	}

	k_memcpy(&page->updated_line, &page->current_line, sizeof(struct k_shell_line));
}

void k_shell_puts(const char *s)
{
	k_shell_update_pages(s);
	k_shell_flush_page_buffers();

	k_fb_update_framebuffer();
}

k_error_t k_shell_init(void)
{
	k_error_t error;

	k_shell.x = 0;
	k_shell.y = 0;

	k_shell.w = k_fb_width();
	if (k_shell.w == K_FB_INVALID_POS)
		return K_ERROR_FAILURE;

	k_shell.h = k_fb_height();
	if (k_shell.h == K_FB_INVALID_POS)
		return K_ERROR_FAILURE;

	k_shell.rows = k_shell.h / K_FONT_CHARACTER_HEIGHT;
	k_shell.cols = k_shell.w / K_FONT_CHARACTER_WIDTH;

	k_shell.scroll = 0;

	error = k_shell_init_page_vector(&k_shell.page, k_shell.rows, k_shell.cols);
	if (error)
		return error;

	return K_ERROR_NONE;
}

