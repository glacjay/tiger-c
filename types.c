#include <assert.h>
#include <stdio.h>

#include "types.h"
#include "utils.h"

static struct type_s _ty_nil = { TY_NIL };
type_t ty_nil(void)
{
    return &_ty_nil;
}

static struct type_s _ty_int = { TY_INT };
type_t ty_int(void)
{
    return &_ty_int;
}

static struct type_s _ty_string = { TY_STRING };
type_t ty_string(void)
{
    return &_ty_string;
}

static struct type_s _ty_void = { TY_VOID };
type_t ty_void(void)
{
    return &_ty_void;
}

type_t ty_record(list_t fields)
{
    type_t p = checked_malloc(sizeof(*p));
    p->kind = TY_RECORD;
    p->u.record = fields;
    return p;
}

type_t ty_array(type_t type)
{
    type_t p = checked_malloc(sizeof(*p));
    p->kind = TY_ARRAY;
    p->u.array = type;
    return p;
}

type_t ty_name(symbol_t name, type_t type)
{
    type_t p = checked_malloc(sizeof(*p));
    p->kind = TY_NAME;
    p->u.name.name = name;
    p->u.name.type = type;
    return p;
}

ty_field_t ty_field(symbol_t name, type_t type)
{
    ty_field_t p = checked_malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    return p;
}

type_t ty_actual(type_t type)
{
    type_t origin = type;
    int counter = 0;

    assert(type);
    while (type->kind == TY_NAME)
    {
        type = type->u.name.type;
        if (++counter > 4096)
            return origin;
    }
    return type;
}

bool ty_match(type_t type1, type_t type2)
{
    type1 = ty_actual(type1);
    type2 = ty_actual(type2);
    if (type1 == type2)
        return true;
    if (type1->kind == TY_RECORD && type2->kind == TY_NIL)
        return true;
    if (type1->kind == TY_NIL && type2->kind == TY_RECORD)
        return true;
    return false;
}

static char _type_strs[][12] = {
    "ty_record", "ty_nil", "ty_int", "ty_string",
    "ty_array", "ty_name", "ty_void",
};

void ty_print(type_t type)
{
    if (type == NULL)
        printf("null");
    else
    {
        printf("%s", _type_strs[type->kind]);
        if (type->kind == TY_NAME)
            printf(", %s", sym_name(type->u.name.name));
    }
}

void ty_print_types(list_t types)
{
    if (types == NULL)
        printf("null");
    else
    {
        printf("ty_list(");
        ty_print(types->data);
        printf(", ");
        ty_print_types(types->next);
        printf(")");
    }
}
