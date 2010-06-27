#ifndef INCLUDE__UTILS_H
#define INCLUDE__UTILS_H

#include <assert.h>

typedef char *string_t;
string_t string(const char *);

typedef char bool;
#define true 1
#define false 0

void *checked_malloc(int);

#endif
