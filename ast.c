#include "ast.h"

ast_decl_t ast_funcs_decl(list_t funcs)
{
    ast_decl_t p = checked_malloc(sizeof(*p));
    p->kind = AST_FUNCS_DECL;
    p->u.funcs = funcs;
    return p;
}

ast_decl_t ast_types_decl(list_t types)
{
    ast_decl_t p = checked_malloc(sizeof(*p));
    p->kind = AST_TYPES_DECL;
    p->u.types = types;
    return p;
}

ast_decl_t ast_var_decl(symbol_t var, symbol_t type, ast_expr_t init)
{
    ast_decl_t p = checked_malloc(sizeof(*p));
    p->kind = AST_VAR_DECL;
    p->u.var.var = var;
    p->u.var.type = type;
    p->u.var.init = init;
    return p;
}

ast_expr_t ast_nil_expr(void)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_NIL_EXPR;
    return p;
}

ast_expr_t ast_var_expr(ast_var_t var)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_VAR_EXPR;
    p->u.var = var;
    return p;
}

ast_expr_t ast_num_expr(int num)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_NUM_EXPR;
    p->u.num = num;
    return p;
}

ast_expr_t ast_string_expr(string_t str)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_STRING_EXPR;
    p->u.str = str;
    return p;
}

ast_expr_t ast_call_expr(symbol_t func, list_t args)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_CALL_EXPR;
    p->u.call.func = func;
    p->u.call.args = args;
    return p;
}

ast_expr_t ast_op_expr(ast_expr_t left, ast_binop_t op, ast_expr_t right)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_OP_EXPR;
    p->u.op.left = left;
    p->u.op.op = op;
    p->u.op.right = right;
    return p;
}

ast_expr_t ast_record_expr(symbol_t type, list_t efields)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_RECORD_EXPR;
    p->u.record.type = type;
    p->u.record.efields = efields;
    return p;
}

ast_expr_t ast_array_expr(symbol_t type, ast_expr_t size, ast_expr_t init)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ARRAY_EXPR;
    p->u.array.type = type;
    p->u.array.size = size;
    p->u.array.init = init;
    return p;
}

ast_expr_t ast_seq_expr(list_t seq)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_SEQ_EXPR;
    p->u.seq = seq;
    return p;
}

ast_expr_t ast_if_expr(ast_expr_t cond, ast_expr_t then, ast_expr_t else_)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_IF_EXPR;
    p->u.if_.cond = cond;
    p->u.if_.then = then;
    p->u.if_.else_ = else_;
    return p;
}

ast_expr_t ast_while_expr(ast_expr_t cond, ast_expr_t body)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_WHILE_EXPR;
    p->u.while_.cond = cond;
    p->u.while_.body = body;
    return p;
}

ast_expr_t ast_for_expr(symbol_t var, ast_expr_t lo, ast_expr_t hi, ast_expr_t body)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_FOR_EXPR;
    p->u.for_.var = var;
    p->u.for_.lo = lo;
    p->u.for_.hi = hi;
    p->u.for_.body = body;
    return p;
}

ast_expr_t ast_break_expr(void)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_BREAK_EXPR;
    return p;
}

ast_expr_t ast_let_expr(list_t decls, ast_expr_t body)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_LET_EXPR;
    p->u.let.decls = decls;
    p->u.let.body = body;
    return p;
}

ast_expr_t ast_assign_expr(ast_var_t var, ast_expr_t expr)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ASSIGN_EXPR;
    p->u.assign.var = var;
    p->u.assign.expr = expr;
    return p;
}

ast_type_t ast_name_type(symbol_t name)
{
    ast_type_t p = checked_malloc(sizeof(*p));
    p->kind = AST_NAME_TYPE;
    p->u.name = name;
    return p;
}

ast_type_t ast_record_type(list_t record)
{
    ast_type_t p = checked_malloc(sizeof(*p));
    p->kind = AST_RECORD_TYPE;
    p->u.record = record;
    return p;
}

ast_type_t ast_array_type(symbol_t array)
{
    ast_type_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ARRAY_TYPE;
    p->u.array = array;
    return p;
}

ast_var_t ast_simple_var(symbol_t simple)
{
    ast_var_t p = checked_malloc(sizeof(*p));
    p->kind = AST_SIMPLE_VAR;
    p->u.simple = simple;
    return p;
}

ast_var_t ast_field_var(ast_var_t var, symbol_t field)
{
    ast_var_t p = checked_malloc(sizeof(*p));
    p->kind = AST_FIELD_VAR;
    p->u.field.var = var;
    p->u.field.field = field;
    return p;
}

ast_var_t ast_sub_var(ast_var_t var, ast_expr_t sub)
{
    ast_var_t p = checked_malloc(sizeof(*p));
    p->kind = AST_SUB_VAR;
    p->u.sub.var = var;
    p->u.sub.sub = sub;
    return p;
}

ast_efield_t ast_efield(symbol_t name, ast_expr_t expr)
{
    ast_efield_t p = checked_malloc(sizeof(*p));
    p->name = name;
    p->expr = expr;
    return p;
}

ast_field_t ast_field(symbol_t name, symbol_t type)
{
    ast_field_t p = checked_malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    return p;
}

ast_func_t ast_func(symbol_t name, list_t params, symbol_t result, ast_expr_t body)
{
    ast_func_t p = checked_malloc(sizeof(*p));
    p->name = name;
    p->params = params;
    p->result = result;
    p->body = body;
    return p;
}

ast_nametype_t ast_nametype(symbol_t name, ast_type_t type)
{
    ast_nametype_t p = checked_malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    return p;
}
