#include "include/i386/spinlock.h"

static void k_spin_lock_loop(volatile int *ptr, k_uint32_t value)
{
	asm volatile("1:\n\t"
		"xor	%%eax, %%eax\n\t"
		"lock; cmpxchgl %0, (%1)\n\t"
		"jnz 1b\n\t"
		: : "r" (value), "r" (ptr)
		: "eax");
}

void k_spin_lock(struct k_spin_lock *lock)
{
	asm volatile("cli");

	k_spin_lock_loop(&lock->lock, 1);
}

void k_spin_unlock(struct k_spin_lock *lock)
{
	lock->lock = 0;

	asm volatile("sti");
}

