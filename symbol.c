#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

#define HT_SIZE 109 /* may be prime */

static symbol_t _symbols[HT_SIZE];

static symbol_t mk_symbol(string_t name, symbol_t next)
{
    symbol_t sym = checked_malloc(sizeof(*sym));
    sym->data = name;
    sym->next = next;
    return sym;
}

static unsigned int hash(string_t str)
{
    unsigned int h = 0;
    char *p;

    for (p = str; *p; p++)
        h = 65599 * h + *p;
    return h;
}

symbol_t symbol(string_t name)
{
    int index = hash(name) % HT_SIZE;
    symbol_t p;

    for (p = _symbols[index]; p; p = p->next)
        if (strcmp((string_t) p->data, name) == 0)
            return p;
    p = mk_symbol(name, _symbols[index]);
    _symbols[index] = p;
    return p;
}

string_t sym_name(symbol_t sym)
{
    return (string_t) sym->data;
}

typedef struct binder_s
{
    symbol_t sym;
    void *value;
    symbol_t prev;
} *binder_t;

struct table_s
{
    list_t table[HT_SIZE];
    symbol_t top;
};

table_t sym_empty(void)
{
    table_t tab = checked_malloc(sizeof(*tab));
    int i;

    tab->top = NULL;
    for (i = 0; i < HT_SIZE; i++)
        tab->table[i] = NULL;
    return tab;
}

static binder_t binder(symbol_t sym, void* value, symbol_t prev)
{
    binder_t b = checked_malloc(sizeof(*b));
    b->sym = sym;
    b->value = value;
    b->prev = prev;
    return b;
}

void sym_enter(table_t tab, symbol_t sym, void *value)
{
    long index;
    binder_t b;

    assert(tab && sym);
    index = ((long) sym) % HT_SIZE;
    b = binder(sym, value, tab->top);
    tab->table[index] = list(b, tab->table[index]);
    tab->top = sym;
}

void *sym_lookup(table_t tab, symbol_t sym)
{
    long index;
    list_t p;

    assert(tab && sym);
    index = ((long) sym) % HT_SIZE;
    for (p = tab->table[index]; p; p = p->next)
    {
        binder_t b = (binder_t) p->data;
        if (b->sym == sym)
            return b->value;
    }
    return NULL;
}

table_t sym_begin_scope(table_t tab)
{
    /* TODO */
    return NULL;
}

void sym_end_scope(table_t tab)
{
    /* TODO */
}
