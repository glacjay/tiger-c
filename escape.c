#include "escape.h"
#include "symbol.h"

typedef struct escape_entry_s *escape_entry_t;
struct escape_entry_s
{
    int depth;
    bool *escape;
};

static escape_entry_t escape_entry(int depth, bool *escape)
{
    assert(escape);
    escape_entry_t p = checked_malloc(sizeof(*p));
    p->depth = depth;
    p->escape = escape;
    *escape = false;
    return p;
}

static int _depth;
static table_t _env;

static void traverse_decl(ast_decl_t decl);
static void traverse_expr(ast_expr_t expr);
static void traverse_var(ast_var_t var);

static void traverse_decl(ast_decl_t decl)
{
    switch (decl->kind)
    {
        case AST_FUNCS_DECL: {
            list_t p = decl->u.funcs;
            for (; p; p = p->next)
            {
                ast_func_t func = p->data;
                list_t q;

                _depth++;
                sym_begin_scope(_env);
                for (q = func->params; q; q = q->next)
                {
                    ast_field_t field = q->data;
                    sym_enter(_env,
                              field->name,
                              escape_entry(_depth, &field->escape));
                }
                traverse_expr(func->body);
                sym_end_scope(_env);
                _depth--;
            }
            break;
        }

        case AST_TYPES_DECL:
            break;

        case AST_VAR_DECL:
            sym_enter(_env,
                      decl->u.var.var,
                      escape_entry(_depth, &decl->u.var.escape));
            traverse_expr(decl->u.var.init);
            break;
    }
}

static void traverse_expr(ast_expr_t expr)
{
    list_t p;

    switch (expr->kind)
    {
        case AST_NIL_EXPR:
            break;

        case AST_VAR_EXPR:
            traverse_var(expr->u.var);
            break;

        case AST_NUM_EXPR:
        case AST_STRING_EXPR:
            break;

        case AST_CALL_EXPR:
            for (p = expr->u.call.args; p; p = p->next)
                traverse_expr(p->data);
            break;

        case AST_OP_EXPR:
            traverse_expr(expr->u.op.left);
            traverse_expr(expr->u.op.right);
            break;

        case AST_RECORD_EXPR:
            for (p = expr->u.record.efields; p; p = p->next)
                traverse_expr(((ast_efield_t) p->data)->expr);
            break;

        case AST_ARRAY_EXPR:
            traverse_expr(expr->u.array.size);
            traverse_expr(expr->u.array.init);
            break;

        case AST_SEQ_EXPR:
            for (p = expr->u.seq; p; p = p->next)
                traverse_expr(p->data);
            break;

        case AST_IF_EXPR:
            traverse_expr(expr->u.if_.cond);
            traverse_expr(expr->u.if_.then);
            if (expr->u.if_.else_)
                traverse_expr(expr->u.if_.else_);
            break;

        case AST_WHILE_EXPR:
            traverse_expr(expr->u.while_.cond);
            traverse_expr(expr->u.while_.body);
            break;

        case AST_FOR_EXPR:
            traverse_expr(expr->u.for_.lo);
            traverse_expr(expr->u.for_.hi);
            sym_begin_scope(_env);
            sym_enter(_env,
                      expr->u.for_.var,
                      escape_entry(_depth, &expr->u.for_.escape));
            traverse_expr(expr->u.for_.body);
            sym_end_scope(_env);
            break;

        case AST_BREAK_EXPR:
            break;

        case AST_LET_EXPR:
            sym_begin_scope(_env);
            for (p = expr->u.let.decls; p; p = p->next)
                traverse_decl(p->data);
            traverse_expr(expr->u.let.body);
            sym_end_scope(_env);
            break;

        case AST_ASSIGN_EXPR:
            traverse_var(expr->u.assign.var);
            traverse_expr(expr->u.assign.expr);
            break;
    }
}

static void traverse_var(ast_var_t var)
{
    switch (var->kind)
    {
        case AST_SIMPLE_VAR: {
            escape_entry_t entry = sym_lookup(_env, var->u.simple);
            if (entry && entry->depth < _depth)
                *entry->escape = true;
            break;
        }

        case AST_FIELD_VAR:
            traverse_var(var->u.field.var);
            break;

        case AST_SUB_VAR:
            traverse_var(var->u.sub.var);
            traverse_expr(var->u.sub.sub);
            break;
    }
}

void esc_find_escape(ast_expr_t expr)
{
    _depth = 0;
    _env = sym_empty();
    traverse_expr(expr);
}
