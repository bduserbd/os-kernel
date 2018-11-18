#ifndef K_TEXT_MODE_VIDEO_H
#define K_TEXT_MODE_VIDEO_H

#include "kernel/include/types.h"
#include "kernel/include/fb/shell.h"

void k_text_puts(const char *);
void k_text_set_info(struct k_fb_info *);

#endif

