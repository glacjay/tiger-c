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

static list_t formal_type_list(list_t params, int pos)
{
    list_t p, q, r;

    q = r = NULL;
    for (p = params; p; p = p->next)
    {
        ast_field_t field = p->data;
        type_t type = sym_lookup(_tenv, field->type);
        if (!type)
        {
            em_error(pos, "undefined type '%s'", field->type);
            type = ty_int();
        }
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

static void trans_decl(ast_decl_t decl)
{
    switch (decl->kind)
    {
        case AST_FUNCS_DECL: {
            list_t p;
            ast_func_t func;

            for (p = decl->u.funcs; p; p = p->next)
            {
                func = p->data;
                list_t formals = formal_type_list(func->params, decl->pos);
                type_t result;

                if (func->result)
                {
                    result = sym_lookup(_tenv, func->result);
                    if (!result)
                    {
                        em_error(decl->pos,
                                 "undefined type '%s'",
                                 sym_name(func->result));
                        result = ty_int();
                    }
                }
                else
                    result = ty_void();
                sym_enter(_venv, func->name, env_func_entry(formals, result));
            }

            for (p = decl->u.funcs; p; p = p->next)
            {
                list_t q, r;
                env_entry_t entry;

                func = p->data;
                entry = sym_lookup(_venv, func->name);
                sym_begin_scope(_venv);
                for (q = func->params, r = entry->u.func.formals;
                     q && r;
                     q = q->next, r = r->next)
                {
                    sym_enter(_venv,
                              ((ast_field_t) q->data)->name,
                              env_var_entry(r->data));
                }
                trans_expr(func->body);
                sym_end_scope(_venv);
            }
            break;
        }

        case AST_TYPES_DECL: {
            list_t p;

            for (p = decl->u.types; p; p = p->next)
            {
                ast_nametype_t nametype = p->data;
                sym_enter(_tenv, nametype->name, ty_name(nametype->name, NULL));
            }
            for (p = decl->u.types; p; p = p->next)
            {
                ast_nametype_t nametype = p->data;
                type_t type = sym_lookup(_tenv, nametype->name);
                type->u.name.type = trans_type(nametype->type);
            }
            break;
        }

        case AST_VAR_DECL: {
            expr_type_t init = trans_expr(decl->u.var.init);
            type_t type = init.type;

            if (decl->u.var.type)
            {
                type = ty_actual(sym_lookup(_tenv, decl->u.var.type));
                if (!type)
                {
                    em_error(decl->pos,
                             "undefined type '%s'",
                             sym_name(decl->u.var.type));
                    type = init.type;
                }
            }
            sym_enter(_venv, decl->u.var.var, env_var_entry(type));
            break;
        }
    }
}

static expr_type_t trans_expr(ast_expr_t expr)
{
    switch (expr->kind)
    {
        case AST_NIL_EXPR:
            return expr_type(NULL, ty_nil());

        case AST_VAR_EXPR:
            return trans_var(expr->u.var);

        case AST_NUM_EXPR:
            return expr_type(NULL, ty_int());

        case AST_STRING_EXPR:
            return expr_type(NULL, ty_string());

        case AST_CALL_EXPR: {
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
                if (ty_actual((type_t) p->data) != et.type)
                    em_error(expr->pos,
                             "passing argument %d of '%s' from wrong type",
                             i,
                             sym_name(expr->u.call.func));
            }
            if (p || q)
                em_error(expr->pos, "wrong number of arguments");
            return expr_type(NULL, ty_actual(entry->u.func.result));
        }

        case AST_OP_EXPR: {
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
                    {
                        em_error(expr->u.op.right->pos, "integer required");
                    }
                    return expr_type(NULL, ty_int());

                case AST_EQ:
                case AST_NEQ:
                    if (left.type == right.type)
                        return expr_type(NULL, ty_int());
                    if (left.type->kind == TY_RECORD && right.type->kind == TY_NIL)
                        return expr_type(NULL, ty_int());
                    if (left.type->kind == TY_NIL && right.type->kind == TY_RECORD)
                        return expr_type(NULL, ty_int());
                    em_error(expr->pos, "the type of two operands must be the same");
                    return expr_type(NULL, ty_int());

                case AST_LT:
                case AST_LE:
                case AST_GT:
                case AST_GE:
                    if (left.type != right.type)
                        em_error(expr->pos,
                                 "the type of two operands must be the same");
                    if (left.type->kind != TY_INT && left.type->kind != TY_STRING)
                        em_error(expr->pos,
                                 "the type of comparison's operand must be int or string");
                    return expr_type(NULL, left.type);
            }

            assert(0);
        }

        case AST_RECORD_EXPR: {
            type_t type = sym_lookup(_tenv, expr->u.record.type);
            list_t p, q;

            if (!type)
            {
                em_error(expr->pos, "undefined type '%s'", sym_name(expr->u.record.type));
                return expr_type(NULL, ty_nil());
            }
            type = ty_actual(type);
            if (type->kind != TY_RECORD)
            {
                em_error(expr->pos,
                         "'%s' is not a record type",
                         sym_name(expr->u.record.type));
            }
            for (p = type->u.record, q = expr->u.record.efields;
                 p && q;
                 p = p->next, q = q->next)
            {
                expr_type_t et = trans_expr(((ast_efield_t) q->data)->expr);
                if (!ty_match(((ty_field_t) p->data)->type, et.type))
                    em_error(((ast_efield_t) q->data)->pos,
                             "wrong field type");
            }
            if (p || q)
                em_error(expr->pos, "wrong field number");
            return expr_type(NULL, type);
        }

        case AST_ARRAY_EXPR: {
            type_t type = ty_actual(sym_lookup(_tenv, expr->u.array.type));
            expr_type_t size = trans_expr(expr->u.array.size);
            expr_type_t init = trans_expr(expr->u.array.init);
            if (type->kind != TY_ARRAY)
                em_error(expr->pos,
                         "'%s' is not array type",
                         sym_name(expr->u.array.type));
            if (size.type->kind != TY_INT)
                em_error(expr->pos, "size must be integer type");
            if (!ty_match(type->u.array, init.type))
            {
                em_error(expr->pos,
                         "initializer expression's type is incorrect");
            }
            return expr_type(NULL, ty_actual(type));
        }

        case AST_SEQ_EXPR: {
            list_t p = expr->u.seq;
            for (; p; p = p->next)
            {
                expr_type_t et = trans_expr((ast_expr_t) p->data);
                if (!p->next)
                    return expr_type(NULL, et.type);
            }
            return expr_type(NULL, ty_void());
        }

        case AST_IF_EXPR: {
            expr_type_t cond = trans_expr(expr->u.if_.cond);
            expr_type_t then = trans_expr(expr->u.if_.then);
            if (cond.type->kind != TY_INT)
                em_error(expr->pos,
                         "the type of if statement's condition must be integer");
            if (expr->u.if_.else_)
            {
                expr_type_t else_ = trans_expr(expr->u.if_.else_);
                if (!ty_match(then.type, else_.type))
                    em_error(expr->pos,
                             "the type of if statement's two substatement must be the same");
            }
            else if (then.type->kind != TY_VOID)
                em_error(expr->pos,
                         "then clause's type must be void if there is no else clause");
            return expr_type(NULL, then.type);
        }

        case AST_WHILE_EXPR: {
            expr_type_t cond = trans_expr(expr->u.while_.cond);
            expr_type_t body = trans_expr(expr->u.while_.body);
            if (cond.type->kind != TY_INT)
                em_error(expr->pos,
                         "the type of while statement's condition must be integer");
            if (body.type->kind != TY_VOID)
                em_error(expr->pos,
                         "the body of while statement must produce no value");
            return expr_type(NULL, ty_void());
        }

        case AST_FOR_EXPR: {
            expr_type_t lo = trans_expr(expr->u.for_.lo);
            expr_type_t hi = trans_expr(expr->u.for_.hi);
            expr_type_t body;
            if (lo.type->kind != TY_INT)
                em_error(expr->pos,
                         "the type of for statement's low range expression must be int");
            if (hi.type->kind != TY_INT)
                em_error(expr->pos,
                         "the type of for statement's high range expression must be int");
            sym_begin_scope(_venv);
            sym_enter(_venv, expr->u.for_.var, env_var_entry(ty_int()));
            body = trans_expr(expr->u.for_.body);
            if (body.type->kind != TY_VOID)
                em_error(expr->pos,
                         "the body of for statement must produce no value");
            sym_end_scope(_venv);
            return expr_type(NULL, ty_void());
        }

        case AST_BREAK_EXPR:
            return expr_type(NULL, ty_void());

        case AST_LET_EXPR: {
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

        case AST_ASSIGN_EXPR: {
            expr_type_t var = trans_var(expr->u.assign.var);
            expr_type_t et = trans_expr(expr->u.assign.expr);
            if (var.type != et.type &&
                (var.type->kind != TY_RECORD || et.type->kind != TY_NIL))
            {
                em_error(expr->pos,
                         "the types of variable and expression are not match");
            }
            /* TODO check assignment to for variable */
            return expr_type(NULL, ty_void());
        }
    }

    assert(0);
}

static type_t trans_type(ast_type_t type)
{
    switch (type->kind)
    {
        case AST_NAME_TYPE: {
            type_t t = sym_lookup(_tenv, type->u.name);
            if (!t)
            {
                em_error(type->pos,
                         "undefined type '%s'",
                         sym_name(type->u.name));
                t = ty_int();
            }
            return t;
        }

        case AST_RECORD_TYPE: {
            list_t p = type->u.record;
            list_t q = NULL, r = NULL;

            for (; p; p = p->next)
            {
                ast_field_t field = p->data;
                type_t t = sym_lookup(_tenv, field->type);

                if (!t)
                {
                    em_error(type->pos,
                             "undefined type '%s'",
                             sym_name(field->type));
                    t = ty_int();
                }
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

        case AST_ARRAY_TYPE: {
            type_t t = sym_lookup(_tenv, type->u.array);
            if (!t)
            {
                em_error(type->pos,
                         "undefined type '%s'",
                         sym_name(type->u.array));
                t = ty_int();
            }
            return ty_array(t);
        }
    }

    assert(0);
}

static expr_type_t trans_var(ast_var_t var)
{
    switch (var->kind)
    {
        case AST_SIMPLE_VAR: {
            env_entry_t entry = sym_lookup(_venv, var->u.simple);
            if (!entry)
            {
                em_error(var->pos,
                         "undefined variable: '%s'",
                         sym_name(var->u.simple));
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

        case AST_FIELD_VAR: {
            expr_type_t et = trans_var(var->u.field.var);
            list_t p;

            if (et.type->kind != TY_RECORD)
            {
                em_error(var->pos, "expected record type variable");
                return expr_type(NULL, ty_int());
            }
            for (p = et.type->u.record; p; p = p->next)
                if (((ty_field_t) p->data)->name == var->u.field.field)
                    return expr_type(NULL,
                                     ty_actual(((ty_field_t) p->data)->type));
            em_error(var->pos,
                     "there is no field named '%s'",
                     sym_name(var->u.field.field));
            return expr_type(NULL, ty_int());
        }

        case AST_SUB_VAR: {
            expr_type_t et = trans_var(var->u.sub.var);
            expr_type_t sub = trans_expr(var->u.sub.sub);
            if (et.type->kind != TY_ARRAY)
            {
                em_error(var->pos, "expected array type variable");
                return expr_type(NULL, ty_int());
            }
            if (sub.type->kind != TY_INT)
            {
                em_error(var->pos, "expected integer type subscript");
                return expr_type(NULL, ty_actual(et.type->u.array));
            }
            return expr_type(NULL, ty_actual(et.type->u.array));
        }
    }

    assert(0);
}

void sem_trans_prog(ast_expr_t prog)
{
    _venv = env_base_venv();
    _tenv = env_base_tenv();
    trans_expr(prog);
}
