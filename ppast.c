#include <assert.h>

#include "ppast.h"
#include "symbol.h"
#include "utils.h"

static void pp_decl(FILE *fp, int d, ast_decl_t decl);
static void pp_type(FILE *fp, int d, ast_type_t type);
static void pp_var(FILE *fp, int d, ast_var_t var);

static void pp_efield(FILE *fp, int d, ast_efield_t efield);
static void pp_field(FILE *fp, int d, ast_field_t field);
static void pp_func(FILE *fp, int d, ast_func_t func);
static void pp_nametype(FILE *fp, int d, ast_nametype_t nametype);

static void indent(FILE *fp, int d)
{
    fprintf(fp, "%*s", d, " ");
}

#define PP_LIST(target, func) \
    do \
    { \
        list_t p = (target); \
        for (; p; p = p->next) \
            func(fp, d+1, p->data); \
    } \
    while (false)

static char _ops[][12] = {
    "PLUS", "MINUS", "TIMES", "DIVIDE",
    "EQ", "NEQ", "LT", "LE", "GT", "GE",
    "AND", "OR"
};

static void pp_op(FILE *fp, ast_binop_t op)
{
    fprintf(fp, "%s", _ops[op]);
}

static void pp_decl(FILE *fp, int d, ast_decl_t decl)
{
    indent(fp, d);
    switch (decl->kind)
    {
        case AST_FUNCS_DECL:
            fprintf(fp, "funcs_decl(\n");
            PP_LIST(decl->u.funcs, pp_func);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_TYPES_DECL:
            fprintf(fp, "types_decl(\n");
            PP_LIST(decl->u.types, pp_nametype);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_VAR_DECL:
            fprintf(fp, "var_decl(%s,\n", sym_name(decl->u.var.var));
            if (decl->u.var.type)
            {
                indent(fp, d+1);
                fprintf(fp, "%s,\n", sym_name(decl->u.var.type));
            }
            pp_expr(fp, d+1, decl->u.var.init);
            fprintf(fp, ")\n");
            break;
        default:
            assert(0);
    }
}

void pp_expr(FILE *fp, int d, ast_expr_t expr)
{
    indent(fp, d);
    switch (expr->kind) {
        case AST_NIL_EXPR:
            fprintf(fp, "nil_expr()");
            break;
        case AST_VAR_EXPR:
            fprintf(fp, "var_expr(\n");
            pp_var(fp, d+1, expr->u.var);
            fprintf(fp, ")");
            break;
        case AST_NUM_EXPR:
            fprintf(fp, "int_expr(%d)", expr->u.num);
            break;
        case AST_STRING_EXPR:
            fprintf(fp, "string_expr(%s)", expr->u.str);
            break;
        case AST_CALL_EXPR:
            fprintf(fp, "call_expr(%s,\n", sym_name(expr->u.call.func));
            PP_LIST(expr->u.call.args, pp_expr);
            fprintf(fp, ")");
            break;
        case AST_OP_EXPR:
            fprintf(fp, "op_expr(\n");
            indent(fp, d+1);
            pp_op(fp, expr->u.op.op);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.op.left);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.op.right);
            fprintf(fp, ")");
            break;
        case AST_RECORD_EXPR:
            fprintf(fp, "record_expr(%s,\n", sym_name(expr->u.record.type));
            PP_LIST(expr->u.record.efields, pp_efield);
            fprintf(fp, ")");
            break;
        case AST_ARRAY_EXPR:
            fprintf(fp, "array_expr(%s,\n", sym_name(expr->u.array.type));
            pp_expr(fp, d+1, expr->u.array.size);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.array.init);
            fprintf(fp, ")");
            break;
        case AST_SEQ_EXPR:
            fprintf(fp, "seq_exp(\n");
            PP_LIST(expr->u.seq, pp_expr);
            fprintf(fp, ")");
            break;
        case AST_IF_EXPR:
            fprintf(fp, "if_expr(\n");
            pp_expr(fp, d+1, expr->u.if_.cond);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.if_.then);
            if (expr->u.if_.else_)
            {
                fprintf(fp, ",\n");
                pp_expr(fp, d+1, expr->u.if_.else_);
            }
            fprintf(fp, ")\n");
            break;
        case AST_WHILE_EXPR:
            fprintf(fp, "while_expr(\n");
            pp_expr(fp, d+1, expr->u.while_.cond);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.while_.body);
            fprintf(fp, ")\n");
            break;
        case AST_FOR_EXPR:
            fprintf(fp, "for_expr(%s,\n", sym_name(expr->u.for_.var));
            pp_expr(fp, d+1, expr->u.for_.lo);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.for_.hi);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.for_.body);
            fprintf(fp, ")\n");
            break;
        case AST_BREAK_EXPR:
            fprintf(fp, "break_expr()");
            break;
        case AST_LET_EXPR:
            fprintf(fp, "let_expr(\n");
            PP_LIST(expr->u.let.decls, pp_decl);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.let.body);
            fprintf(fp, ")\n");
            break;
        case AST_ASSIGN_EXPR:
            fprintf(fp, "assign_expr(\n");
            pp_var(fp, d+1, expr->u.assign.var);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, expr->u.assign.expr);
            fprintf(fp, ")\n");
            break;
        default:
            assert(0);
    }
}

static void pp_type(FILE *fp, int d, ast_type_t type)
{
    indent(fp, d);
    switch (type->kind)
    {
        case AST_NAME_TYPE:
            fprintf(fp, "name_type(%s)", sym_name(type->u.name));
            break;
        case AST_RECORD_TYPE:
            fprintf(fp, "record_type(\n");
            PP_LIST(type->u.record, pp_field);
            fprintf(fp, ")\n");
            break;
        case AST_ARRAY_TYPE:
            fprintf(fp, "array_type(%s)", sym_name(type->u.array));
            break;
        default:
            assert(0);
    }
}

static void pp_var(FILE *fp, int d, ast_var_t var)
{
    indent(fp, d);
    switch (var->kind)
    {
        case AST_SIMPLE_VAR:
            fprintf(fp, "simple_var(%s)", sym_name(var->u.simple));
            break;
        case AST_FIELD_VAR:
            fprintf(fp, "field_var(\n");
            pp_var(fp, d+1, var->u.field.var);
            fprintf(fp, ",\n");
            indent(fp, d+1);
            fprintf(fp, "%s)\n", sym_name(var->u.field.field));
            break;
        case AST_SUB_VAR:
            fprintf(fp, "sub_var(\n");
            pp_var(fp, d+1, var->u.sub.var);
            fprintf(fp, ",\n");
            pp_expr(fp, d+1, var->u.sub.sub);
            fprintf(fp, ")\n");
            break;
        default:
            assert(0);
    }
}

static void pp_efield(FILE *fp, int d, ast_efield_t efield)
{
    indent(fp, d);
    if (efield)
    {
        fprintf(fp, "efield(%s,\n", sym_name(efield->name));
        pp_expr(fp, d+1, efield->expr);
        fprintf(fp, ")\n");
    }
    else
        fprintf(fp, "efield()");
}

static void pp_field(FILE *fp, int d, ast_field_t field)
{
    indent(fp, d);
    fprintf(fp, "field(%s,\n", sym_name(field->name));
    indent(fp, d+1);
    fprintf(fp, "%s)\n", sym_name(field->type));
}

static void pp_func(FILE *fp, int d, ast_func_t func)
{
    indent(fp, d);
    fprintf(fp, "func(%s,\n", sym_name(func->name));
    PP_LIST(func->params, pp_field);
    fprintf(fp, ",\n");
    if (func->result)
    {
        indent(fp, d+1);
        fprintf(fp, "%s,\n", sym_name(func->result));
    }
    pp_expr(fp, d+1, func->body);
    fprintf(fp, ")\n");
}

static void pp_nametype(FILE *fp, int d, ast_nametype_t nametype)
{
    indent(fp, d);
    fprintf(fp, "nametype(%s,\n", sym_name(nametype->name));
    pp_type(fp, d+1, nametype->type);
    fprintf(fp, ")\n");
}
