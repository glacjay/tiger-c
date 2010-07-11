#ifndef INCLUDE__TYPES_H
#define INCLUDE__TYPES_H

#include "symbol.h"

typedef struct type_s *type_t;
typedef struct ty_field_s *ty_field_t;

struct type_s
{
    enum { TY_RECORD, TY_NIL, TY_INT, TY_STRING, TY_ARRAY, TY_NAME, TY_VOID } kind;
    union
    {
        list_t record;
        type_t array;
        struct { symbol_t name; type_t type; } name;
    } u;
};
type_t ty_nil(void);
type_t ty_int(void);
type_t ty_string(void);
type_t ty_void(void);
type_t ty_record(list_t fields);
type_t ty_array(type_t type);
type_t ty_name(symbol_t name, type_t type);

struct ty_field_s
{
    symbol_t name;
    type_t type;
};
ty_field_t ty_field(symbol_t name, type_t type);

type_t ty_actual(type_t type);
bool ty_match(type_t type1, type_t type2);

void ty_print(type_t type);
void ty_print_types(list_t types);

#endif
