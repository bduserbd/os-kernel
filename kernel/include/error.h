#ifndef K_ERROR_H
#define K_ERROR_H

typedef enum {
	K_ERROR_NONE = 0,
	K_ERROR_FAILURE,
	K_ERROR_INVALID_PARAMETER,
	K_ERROR_UNINITIALIZED,
	K_ERROR_BAD_CHECKSUM,
	K_ERROR_INVALID_DEVICE,
	K_ERROR_NOT_FOUND,
	K_ERROR_MEMORY_ALLOCATION_FAILED,
	K_ERROR_INVALID_MODULE_HEADER,
	K_ERROR_INVALID_MODULE_SIZE,
	K_ERROR_INVALID_MODULE_INFO,
	K_ERROR_INVALID_MODULE_ELF,
	K_ERROR_MODULE_ALREADY_EXISTS,
	K_ERROR_UNEXPORTED_SYMBOL,
} k_error_t;

#endif

