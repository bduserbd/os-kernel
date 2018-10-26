#ifndef K_IO_H
#define K_IO_H

#include "kernel/include/types.h"

void k_outb(k_uint8_t, k_uint16_t);
void k_outw(k_uint16_t, k_uint16_t);
void k_outl(k_uint32_t, k_uint16_t);

k_uint8_t k_inb(k_uint16_t);
k_uint16_t k_inw(k_uint16_t);
k_uint32_t k_inl(k_uint16_t); 

void k_io_wait(void);

#endif

