#ifndef INCLUDE__UTILS_H
#define INCLUDE__UTILS_H

#include <assert.h>

typedef char *string_t;
string_t string(const char *);

typedef char bool;
#define true 1
#define false 0

#define HT_SIZE 773

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
list_t vlist(int count, ...);
list_t int_list(int i, list_t next);
list_t bool_list(bool b, list_t next);

list_t join_list(list_t list1, list_t list2);
list_t list_append(list_t list1, void *data);

#endif
