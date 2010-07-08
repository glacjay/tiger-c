#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

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

table_t sym_empty(void)
{
    return tab_empty();
}

void sym_enter(table_t tab, symbol_t sym, void *value)
{
    tab_enter(tab, sym, value);
}

void *sym_lookup(table_t tab, symbol_t sym)
{
    return tab_lookup(tab, sym);
}

static symbol_t _mark_sym = NULL;

void sym_begin_scope(table_t tab)
{
    if (!_mark_sym)
    {
        _mark_sym = checked_malloc(sizeof(*_mark_sym));
        _mark_sym->data = "<mark>";
        _mark_sym->next = NULL;
    }
    sym_enter(tab, _mark_sym, NULL);
}

void sym_end_scope(table_t tab)
{
    symbol_t sym;

    do
        sym = tab_pop(tab);
    while (sym != _mark_sym);
}
