#ifndef K_FONT_H
#define K_FONT_H

#include "kernel/include/types.h"

/* Character dimensions in pixels */
#define K_FONT_CHARACTER_WIDTH		8
#define K_FONT_CHARACTER_HEIGHT	16

/* ASCII only ! */
#define K_FONT_ASCII_CHARACTERS	128

extern const k_uint64_t k_font_lookup_table2[16];
extern const k_uint64_t k_font_lookup_table4[4];

extern const k_uint8_t k_font[K_FONT_ASCII_CHARACTERS][K_FONT_CHARACTER_HEIGHT];

#endif

