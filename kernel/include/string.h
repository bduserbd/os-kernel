#ifndef K_STRING_H
#define K_STRING_H

#include "kernel/include/types.h"

void *k_memset(void *, int, k_size_t);
void *k_memcpy(void *, const void *, k_size_t);
int k_memcmp(const void *, const void *, k_size_t);
int k_strcmp(const char *, const char *);
int k_strncmp(const char *, const char *, k_size_t);
char *k_strcpy(char *, const char *);
char *k_strncpy(char *, const char *, k_size_t);
k_size_t k_strlen(const char *);
k_size_t k_strnlen(const char *, k_size_t);
char *k_strcat(char *, const char *);
char *k_strncat(char *, const char *, k_size_t);
int k_isprint(int);
int k_isspace(int);
int k_isalpha(int);
int k_isdigit(int);
int k_isxdigit(int);
int k_islower(int);
int k_isupper(int);
int k_toupper(int);
int k_tolower(int);

#endif

