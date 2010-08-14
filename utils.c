#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

string_t string(const char *str)
{
    string_t p = checked_malloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

void *checked_malloc(int size)
{
    void *p = malloc(size);
    assert(p);
    return p;
}

list_t list(void *data, list_t next)
{
    list_t p = checked_malloc(sizeof(*p));
    p->data = data;
    p->next = next;
    return p;
}

list_t vlist(int count, ...)
{
    list_t result = NULL, next = NULL;
    va_list ap;

    va_start(ap, count);
    for (; count > 0; count--)
    {
        void *data = va_arg(ap, void *);
        list_t p = list(data, NULL);
        if (result)
            next = next->next = p;
        else
            result = next = p;
    }
    return result;
}

list_t int_list(int i, list_t next)
{
    list_t p = checked_malloc(sizeof(*p));
    p->i = i;
    p->next = next;
    return p;
}

list_t bool_list(bool b, list_t next)
{
    list_t p = checked_malloc(sizeof(*p));
    p->b = b;
    p->next = next;
    return p;
}

list_t join_list(list_t list1, list_t list2)
{
    list_t p = list1;
    if (!p)
        return list2;
    while (p->next)
        p = p->next;
    p->next = list2;
    return list1;
}
