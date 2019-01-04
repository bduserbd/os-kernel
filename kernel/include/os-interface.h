#ifndef K_OS_INTERFACE_H
#define K_OS_INTERFACE_H

#include "kernel/include/types.h"

typedef k_size_t	size_t;

void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);
int memcmp(const void *, const void *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
int isprint(int);
int isspace(int);
int isalpha(int);
int isdigit(int);
int isxdigit(int);
int toupper(int);
int tolower(int);

#endif

