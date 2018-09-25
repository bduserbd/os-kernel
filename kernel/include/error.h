#ifndef K_ERROR_H
#define K_ERROR_H

typedef enum {
	K_ERROR_NONE = 0,
	K_ERROR_FAILURE,
	K_ERROR_BAD_CHECKSUM,
	K_ERROR_INVALID_DEVICE,
	K_ERROR_NOT_FOUND,
} k_error_t;

#endif

