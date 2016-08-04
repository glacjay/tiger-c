#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "table.h"
#include "utils.h"

typedef struct binder_s *binder_t;
struct binder_s
{
    void *key;
    void *value;
    binder_t prev;
};

struct table_s
{
    list_t table[HT_SIZE];
    binder_t top;
};

static binder_t binder(void *key, void *value, binder_t prev)
{
    binder_t p = checked_malloc(sizeof(*p));
    p->key = key;
    p->value = value;
    p->prev = prev;
    return p;
}

table_t tab_empty(void)
{
    table_t p = checked_malloc(sizeof(*p));
    int i;

    p->top = NULL;
    for (i = 0; i < HT_SIZE; i++)
        p->table[i] = NULL;
    return p;
}

void tab_enter(table_t tab, void *key, void *value)
{
    int index;

    assert(tab && key);
    index = ((intptr_t) key) % HT_SIZE;
    tab->table[index] = list(binder(key, value, tab->top), tab->table[index]);
    tab->top = tab->table[index]->data;
}

void *tab_lookup(table_t tab, void *key)
{
    int index;
    list_t p;

    assert(tab && key);
    index = ((intptr_t) key) % HT_SIZE;
    for (p = tab->table[index]; p; p = p->next)
        if (((binder_t) p->data)->key == key)
            return ((binder_t) p->data)->value;
    return NULL;
}

void *tab_pop(table_t tab)
{
    binder_t bind;
    list_t p;
    int index;

    assert(tab);
    bind = tab->top;
    assert(bind);
    index = ((intptr_t) bind->key) % HT_SIZE;
    p = tab->table[index];
    assert(p);
    tab->table[index] = p->next;
    tab->top = bind->prev;
    return bind->key;
}

void tab_dump(table_t tab, tab_dump_func_t show)
{
    binder_t bind = tab->top;

    for (; bind; bind = bind->prev)
        show(bind->key, bind->value);
}
