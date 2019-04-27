#ifndef K_ARCH_H
#define K_ARCH_H

#ifdef K_BITS_32

#include "i386/spinlock.h"

#elif K_BITS_64

#include "x86_64/spinlock.h"

#endif

#endif

