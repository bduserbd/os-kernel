#ifndef K_X86_64_SPINLOCK_H
#define K_X86_64_SPINLOCK_H

#include "kernel/include/types.h"

struct k_spin_lock {
	volatile int lock;
};

#endif

