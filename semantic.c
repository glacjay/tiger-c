#include <stdlib.h>

#include "env.h"
#include "errmsg.h"
#include "ppast.h"
#include "semantic.h"
#include "types.h"

typedef void *tr_expr_t;

static table_t _venv;
static table_t _tenv;

typedef struct expr_type_s expr_type_t;
struct expr_type_s
{
    tr_expr_t expr;
    type_t type;
};

static expr_type_t expr_type(tr_expr_t expr, type_t type)
{
    expr_type_t result;
    result.expr = expr;
    result.type = type;
    return result;
}

static void trans_decl(ast_decl_t decl);
static expr_type_t trans_expr(ast_expr_t expr);
static type_t trans_type(ast_type_t type);
static expr_type_t trans_var(ast_var_t var);

#if 0
static void show_types(void *key, void *value)
{
    string_t name = sym_name((symbol_t) key);
    type_t type = value;
    printf("name: %s; type: ", name);
    ty_print(type);
    printf("\n");
}
#endif

static type_t lookup_type(symbol_t name, int pos)
{
    type_t type = sym_lookup(_tenv, name);
    if (type)
        type = ty_actual(type);
    else
        em_error(pos, "undefined type '%s'", sym_name(name));
    return type;
}

static list_t formal_type_list(list_t params, int pos)
{
    list_t p, q = NULL, r = NULL;

    for (p = params; p; p = p->next)
    {
        ast_field_t field = p->data;
        type_t type = lookup_type(field->type, pos);
        if (!type)
            type = ty_int();
        if (r)
        {
            r->next = list(type, NULL);
            r = r->next;
        }
        else
            q = r = list(type, NULL);
    }
    return q;
}

static void trans_funcs_decl(ast_decl_t decl)
{
    list_t p;

    /* Enter function prototypes into symbol table. */
    for (p = decl->u.funcs; p; p = p->next)
    {
        ast_func_t func = p->data;
        list_t formals = formal_type_list(func->params, decl->pos);
        type_t result;

        if (func->result)
        {
            result = lookup_type(func->result, decl->pos);
            if (!result)
                result = ty_int();
        }
        else
            result = ty_void();
        sym_enter(_venv, func->name, env_func_entry(formals, result));
    }

    /* Translate the possibly mutually recursive functions. */
    for (p = decl->u.funcs; p; p = p->next)
    {
        ast_func_t func = p->data;
        env_entry_t entry = sym_lookup(_venv, func->name);
        list_t q = func->params;
        list_t r = entry->u.func.formals;

        sym_begin_scope(_venv);
        for (; q; q = q->next, r = r->next)
        {
            sym_enter(_venv,
                      ((ast_field_t) q->data)->name,
                      env_var_entry(r->data, false));
        }
        assert(!q && !r);
        trans_expr(func->body);
        sym_end_scope(_venv);
    }
}

static void trans_types_decl(ast_decl_t decl)
{
    list_t p;

    /* Enter type placeholder into symbol table. */
    for (p = decl->u.types; p; p = p->next)
    {
        ast_nametype_t nametype = p->data;
        sym_enter(_tenv, nametype->name, ty_name(nametype->name, NULL));
    }

    /* Translate possibly mutually recursive types. */
    for (p = decl->u.types; p; p = p->next)
    {
        ast_nametype_t nametype = p->data;
        type_t type = sym_lookup(_tenv, nametype->name);
        type->u.name.type = trans_type(nametype->type);
    }

    /* Check for infinite recursive types. */
    for (p = decl->u.types; p; p = p->next)
    {
        ast_nametype_t nametype = p->data;
        type_t type = sym_lookup(_tenv, nametype->name);
        if (ty_actual(type) == type)
            em_error(decl->pos,
                     "infinite recursive type '%s'",
                     sym_name(nametype->name));
    }
}

static void trans_var_decl(ast_decl_t decl)
{
    expr_type_t init = trans_expr(decl->u.var.init);
    type_t type = init.type;

    if (decl->u.var.type)
    {
        type = lookup_type(decl->u.var.type, decl->pos);
        if (!type)
            type = ty_int();
        if (!ty_match(type, init.type))
            em_error(decl->pos,
                     "initializer has incorrect type");
    }
    sym_enter(_venv, decl->u.var.var, env_var_entry(type, false));
}

typedef void (*trans_decl_func)(ast_decl_t);
/* XXX Keep sync with ast_decl_t's declaration! */
static trans_decl_func _trans_decl_funcs[] =
{
    trans_funcs_decl,
    trans_types_decl,
    trans_var_decl,
};

static void trans_decl(ast_decl_t decl)
{
    _trans_decl_funcs[decl->kind](decl);
}

static expr_type_t trans_nil_expr(ast_expr_t expr)
{
    return expr_type(NULL, ty_nil());
}

static expr_type_t trans_var_expr(ast_expr_t expr)
{
    return trans_var(expr->u.var);
}

static expr_type_t trans_num_expr(ast_expr_t expr)
{
    return expr_type(NULL, ty_int());
}

static expr_type_t trans_string_expr(ast_expr_t expr)
{
    return expr_type(NULL, ty_string());
}

static expr_type_t trans_call_expr(ast_expr_t expr)
{
    env_entry_t entry = sym_lookup(_venv, expr->u.call.func);
    list_t p, q;
    int i;

    if (!entry)
    {
        em_error(expr->pos,
                 "undefined function '%s'",
                 sym_name(expr->u.call.func));
        return expr_type(NULL, ty_int());
    }
    else if (entry->kind != ENV_FUNC_ENTRY)
    {
        em_error(expr->pos,
                 "'%s' is not a function",
                 sym_name(expr->u.call.func));
        return expr_type(NULL, ty_int());
    }

    for (p = entry->u.func.formals, q = expr->u.call.args, i = 1;
         p && q;
         p = p->next, q = q->next, i++)
    {
        expr_type_t et = trans_expr((ast_expr_t) q->data);
        if (!ty_match(p->data, et.type))
            em_error(expr->pos,
                     "passing argument %d of '%s' with wrong type",
                     i,
                     sym_name(expr->u.call.func));
    }
    if (p)
        em_error(expr->pos, "expect more arguments");
    else if (q)
        em_error(expr->pos, "expect less arguments");
    return expr_type(NULL, ty_actual(entry->u.func.result));
}

static expr_type_t trans_op_expr(ast_expr_t expr)
{
    ast_binop_t op = expr->u.op.op;
    expr_type_t left = trans_expr(expr->u.op.left);
    expr_type_t right = trans_expr(expr->u.op.right);

    switch (op) {
        case AST_PLUS:
        case AST_MINUS:
        case AST_TIMES:
        case AST_DIVIDE:
        case AST_AND:
        case AST_OR:
            if (left.type->kind != TY_INT)
                em_error(expr->u.op.left->pos, "integer required");
            if (right.type->kind != TY_INT)
                em_error(expr->u.op.right->pos, "integer required");
            return expr_type(NULL, ty_int());

        case AST_EQ:
        case AST_NEQ:
            if (!ty_match(left.type, right.type))
                em_error(expr->pos,
                         "the type of two operands must be the same");
            return expr_type(NULL, ty_int());

        case AST_LT:
        case AST_LE:
        case AST_GT:
        case AST_GE:
            if (!ty_match(left.type, right.type))
                em_error(expr->pos,
                         "the type of two operands must be the same");
            if (left.type->kind != TY_INT && left.type->kind != TY_STRING)
                em_error(expr->pos,
                         "the type of comparison's operand must be int or string");
            return expr_type(NULL, left.type);
    }

    assert(0);
}

static expr_type_t trans_record_expr(ast_expr_t expr)
{
    type_t type = lookup_type(expr->u.record.type, expr->pos);
    list_t p, q;

    if (!type)
        return expr_type(NULL, ty_nil());
    if (type->kind != TY_RECORD)
        em_error(expr->pos,
                 "'%s' is not a record type",
                 sym_name(expr->u.record.type));
    for (p = type->u.record, q = expr->u.record.efields;
         p && q;
         p = p->next, q = q->next)
    {
        ast_efield_t efield = q->data;
        expr_type_t et = trans_expr(efield->expr);
        if (!ty_match(((ty_field_t) p->data)->type, et.type))
            em_error(efield->pos, "wrong field type");
    }
    if (p || q)
        em_error(expr->pos, "wrong field number");
    return expr_type(NULL, type);
}

static expr_type_t trans_array_expr(ast_expr_t expr)
{
    type_t type = lookup_type(expr->u.array.type, expr->pos);
    expr_type_t size = trans_expr(expr->u.array.size);
    expr_type_t init = trans_expr(expr->u.array.init);

    if (!type)
        return expr_type(NULL, ty_int());
    if (type->kind != TY_ARRAY)
        em_error(expr->pos,
                 "'%s' is not an array type",
                 sym_name(expr->u.array.type));
    if (size.type->kind != TY_INT)
        em_error(expr->pos, "array's size must be the int type");
    if (!ty_match(type->u.array, init.type))
        em_error(expr->pos, "initializer has incorrect type");
    return expr_type(NULL, type);
}

static expr_type_t trans_seq_expr(ast_expr_t expr)
{
    list_t p = expr->u.seq;
    for (; p; p = p->next)
    {
        expr_type_t et = trans_expr((ast_expr_t) p->data);
        if (!p->next)
            return expr_type(NULL, et.type);
    }
    return expr_type(NULL, ty_void());
}

static expr_type_t trans_if_expr(ast_expr_t expr)
{
    expr_type_t cond = trans_expr(expr->u.if_.cond);
    expr_type_t then = trans_expr(expr->u.if_.then);

    if (cond.type->kind != TY_INT)
        em_error(expr->pos,
                 "condition's type must be integer");
    if (expr->u.if_.else_)
    {
        expr_type_t else_ = trans_expr(expr->u.if_.else_);
        if (!ty_match(then.type, else_.type))
            em_error(expr->pos, "types of then and else differ");
        return expr_type(NULL, then.type);
    }
    else if (then.type->kind != TY_VOID)
        em_error(expr->pos, "if-then should return nothing");
    return expr_type(NULL, ty_void());
}

static expr_type_t trans_while_expr(ast_expr_t expr)
{
    expr_type_t cond = trans_expr(expr->u.while_.cond);
    expr_type_t body = trans_expr(expr->u.while_.body);
    if (cond.type->kind != TY_INT)
        em_error(expr->pos, "condition's type must be integer");
    if (body.type->kind != TY_VOID)
        em_error(expr->pos, "while should return nothing");
    return expr_type(NULL, ty_void());
}

static expr_type_t trans_for_expr(ast_expr_t expr)
{
    expr_type_t lo = trans_expr(expr->u.for_.lo);
    expr_type_t hi = trans_expr(expr->u.for_.hi);
    expr_type_t body;
    if (lo.type->kind != TY_INT)
        em_error(expr->pos, "lo expression should be int type");
    if (hi.type->kind != TY_INT)
        em_error(expr->pos, "hi expression should be int type");
    sym_begin_scope(_venv);
    sym_enter(_venv, expr->u.for_.var, env_var_entry(ty_int(), true));
    /* TODO Check assignment to the variable. */
    body = trans_expr(expr->u.for_.body);
    if (body.type->kind != TY_VOID)
        em_error(expr->pos, "for should return nothing");
    sym_end_scope(_venv);
    return expr_type(NULL, ty_void());
}

static expr_type_t trans_break_expr(ast_expr_t expr)
{
    /* TODO Check for outer for or while statement. */
    return expr_type(NULL, ty_void());
}

static expr_type_t trans_let_expr(ast_expr_t expr)
{
    expr_type_t result;
    list_t p;

    sym_begin_scope(_venv);
    sym_begin_scope(_tenv);
    for (p = expr->u.let.decls; p; p = p->next)
        trans_decl(p->data);
    result = trans_expr(expr->u.let.body);
    sym_end_scope(_venv);
    sym_end_scope(_tenv);
    return result;
}

static expr_type_t trans_assign_expr(ast_expr_t expr)
{
    expr_type_t var = trans_var(expr->u.assign.var);
    expr_type_t et = trans_expr(expr->u.assign.expr);

    if (!ty_match(var.type, et.type))
        em_error(expr->pos, "type mismatch");

    if (expr->u.assign.var->kind == AST_SIMPLE_VAR && var.type->kind == TY_INT)
    {
        /* Check for the assignment to the for variable. */
        ast_var_t v = expr->u.assign.var;
        env_entry_t entry = sym_lookup(_venv, v->u.simple);
        if (entry && entry->kind == ENV_VAR_ENTRY && entry->u.var.for_)
            em_error(expr->pos, "assigning to the for variable");
    }

    return expr_type(NULL, ty_void());
}

typedef expr_type_t (*trans_expr_func)(ast_expr_t);
static trans_expr_func _trans_expr_funcs[] =
{
    trans_nil_expr,
    trans_var_expr,
    trans_num_expr,
    trans_string_expr,
    trans_call_expr,
    trans_op_expr,
    trans_record_expr,
    trans_array_expr,
    trans_seq_expr,
    trans_if_expr,
    trans_while_expr,
    trans_for_expr,
    trans_break_expr,
    trans_let_expr,
    trans_assign_expr,
};

static expr_type_t trans_expr(ast_expr_t expr)
{
    return _trans_expr_funcs[expr->kind](expr);
}

static type_t trans_name_type(ast_type_t type)
{
    type_t t = lookup_type(type->u.name, type->pos);
    if (!t)
        t = ty_int();
    return t;
}

static type_t trans_record_type(ast_type_t type)
{
    list_t p = type->u.record;
    list_t q = NULL, r = NULL;

    for (; p; p = p->next)
    {
        ast_field_t field = p->data;
        type_t t = lookup_type(field->type, type->pos);

        if (!t)
            t = ty_int();
        if (r)
        {
            r->next = list(ty_field(field->name, t), NULL);
            r = r->next;
        }
        else
            q = r = list(ty_field(field->name, t), NULL);
    }
    return ty_record(q);
}

static type_t trans_array_type(ast_type_t type)
{
    type_t t = lookup_type(type->u.array, type->pos);
    if (!t)
        t = ty_int();
    return ty_array(t);
}

typedef type_t (*trans_type_func)(ast_type_t);
static trans_type_func _trans_type_funcs[] =
{
    trans_name_type,
    trans_record_type,
    trans_array_type,
};

static type_t trans_type(ast_type_t type)
{
    return _trans_type_funcs[type->kind](type);
}

static expr_type_t trans_simple_var(ast_var_t var)
{
    env_entry_t entry = sym_lookup(_venv, var->u.simple);
    if (!entry)
    {
        em_error(var->pos, "undefined variable '%s'", sym_name(var->u.simple));
        return expr_type(NULL, ty_int());
    }
    else if (entry->kind != ENV_VAR_ENTRY)
    {
        em_error(var->pos,
                 "expected '%s' to be a variable, not a function",
                 sym_name(var->u.simple));
        return expr_type(NULL, ty_int());
    }
    else
        return expr_type(NULL, ty_actual(entry->u.var.type));
}

static expr_type_t trans_field_var(ast_var_t var)
{
    expr_type_t et = trans_var(var->u.field.var);
    list_t p;

    if (et.type->kind != TY_RECORD)
    {
        em_error(var->pos, "expected record type variable");
        return expr_type(NULL, ty_int());
    }
    for (p = et.type->u.record; p; p = p->next)
    {
        ty_field_t field = p->data;
        if (field->name == var->u.field.field)
            return expr_type(NULL, ty_actual(field->type));
    }
    em_error(var->pos,
             "there is no field named '%s'",
             sym_name(var->u.field.field));
    return expr_type(NULL, ty_int());
}

static expr_type_t trans_sub_var(ast_var_t var)
{
    expr_type_t et = trans_var(var->u.sub.var);
    expr_type_t sub = trans_expr(var->u.sub.sub);

    if (et.type->kind != TY_ARRAY)
    {
        em_error(var->pos, "expected array type variable");
        return expr_type(NULL, ty_int());
    }
    if (sub.type->kind != TY_INT)
        em_error(var->pos, "expected integer type subscript");
    return expr_type(NULL, ty_actual(et.type->u.array));
}

typedef expr_type_t (*trans_var_func)(ast_var_t);
static trans_var_func _trans_var_funcs[] =
{
    trans_simple_var,
    trans_field_var,
    trans_sub_var,
};

static expr_type_t trans_var(ast_var_t var)
{
    return _trans_var_funcs[var->kind](var);
}

void sem_trans_prog(ast_expr_t prog)
{
    _venv = env_base_venv();
    _tenv = env_base_tenv();
    trans_expr(prog);
}
