#ifndef INCLUDE__UTILS_H
#define INCLUDE__UTILS_H

#include <assert.h>

typedef char *string_t;
string_t string(const char *);

typedef char bool;
#define true 1
#define false 0

void *checked_malloc(int);

typedef struct list_s *list_t;
struct list_s
{
    union {
        int i;
        bool b;
        void *data;
    };
    struct list_s *next;
};
list_t list(void *data, list_t next);

#endif
