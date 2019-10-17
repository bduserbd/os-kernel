#ifndef K_SPINLOCK_H
#define K_SPINLOCK_H

#include "arch.h"

typedef struct k_spin_lock k_spin_lock_t;

void k_spin_lock(k_spin_lock_t *);
void k_spin_unlock(k_spin_lock_t *);
int k_spin_is_locked(k_spin_lock_t *);

#endif

