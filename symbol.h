#ifndef INCLUDE__SYMBOL_H
#define INCLUDE__SYMBOL_H

#include "table.h"
#include "utils.h"

typedef list_t symbol_t;

symbol_t symbol(string_t name);
string_t sym_name(symbol_t sym);
table_t sym_empty(void);
void sym_enter(table_t tab, symbol_t sym, void *value);
void *sym_lookup(table_t tab, symbol_t sym);
void sym_begin_scope(table_t tab);
void sym_end_scope(table_t tab);

#endif
