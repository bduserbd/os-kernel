#ifndef K_TASK_H
#define K_TASK_H

#include "kernel/include/types.h"
#include "kernel/include/error.h"

#define K_TASK_FIRST_PID	0

typedef k_uint16_t	k_pid_t;

enum {
	K_TASK_STATE_UNINITIALIZED,
	K_TASK_STATE_RUNNING,
	K_TASK_STATE_WAITING,
	K_TASK_STATE_SLEEPING,
	K_TASK_STATE_STOPPED,
};

#define K_TASK_STACK_SIZE	0x1000

#define K_TASK_DEFAULT_PRIORITY	50

typedef k_error_t (*k_task_entry_point_t)(void *);

struct k_task {
	k_pid_t pid;

	int state;

	int priority;

	k_task_entry_point_t func;
	void *stack;

	void *arch;

	//struct k_task *next;
};

extern struct k_task *k_task;

void k_task_init(void);
void k_task_create(k_task_entry_point_t, void *);
void k_task_switch(void);

struct k_task *k_task_get_current(void);

void k_schedule(void);

int k_get_cpu_number(void);

#endif

