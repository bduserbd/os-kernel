#ifndef K_SYSTEM_CALL_H
#define K_SYSTEM_CALL_H

enum {
	K_SYSTEM_CALL_EXIT	= 1,
	K_SYSTEM_CALL_FORK	= 2,
	K_SYSTEM_CALL_READ	= 3,
	K_SYSTEM_CALL_WRITE	= 4,
	K_SYSTEM_CALL_OPEN	= 5,
	K_SYSTEM_CALL_CLOSE	= 6,
	K_SYSTEM_CALL_GETPID	= 20,
	K_SYSTEM_CALL_TOTAL,
};

#endif

