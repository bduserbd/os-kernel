#ifndef K_IO_H
#define K_IO_H

#include "kernel/include/types.h"

static inline void k_outb(k_uint8_t v, k_uint16_t port)
{
	asm volatile("outb %0,%1" : : "a" (v), "dN" (port));
}

static inline k_uint8_t k_inb(k_uint16_t port)
{
	k_uint8_t v;

	asm volatile("inb %1,%0" : "=a" (v) : "dN" (port));

	return v;
}

static inline void k_outw(k_uint16_t v, k_uint16_t port)
{
	asm volatile("outw %0,%1" : : "a" (v), "dN" (port));
}

static inline k_uint16_t k_inw(k_uint16_t port)
{
	k_uint16_t v;

	asm volatile("inw %1,%0" : "=a" (v) : "dN" (port));

	return v;
}

static inline void k_outl(k_uint32_t v, k_uint16_t port)
{
	asm volatile("outl %0,%1" : : "a" (v), "dN" (port));
}

static inline k_uint32_t k_inl(k_uint16_t port)
{
	k_uint32_t v;

	asm volatile("inl %1,%0" : "=a" (v) : "dN" (port));

	return v;
}

#endif

