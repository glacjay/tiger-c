#include <assert.h>

#include "ppast.h"
#include "symbol.h"
#include "utils.h"

static void pp_efield(FILE *fp, int d, ast_efield_t efield);
static void pp_field(FILE *fp, int d, ast_field_t field);
static void pp_func(FILE *fp, int d, ast_func_t func);
static void pp_nametype(FILE *fp, int d, ast_nametype_t nametype);

static void indent(FILE *fp, int d)
{
    fprintf(fp, "%*s", d+1, " ");
}

static char _ops[][12] = {
    "PLUS", "MINUS", "TIMES", "DIVIDE",
    "EQ", "NEQ", "LT", "LE", "GT", "GE",
    "AND", "OR"
};

static void pp_op(FILE *fp, ast_binop_t op)
{
    fprintf(fp, "%s\n", _ops[op]);
}

typedef void (*pp_func_t)(FILE *, int, void *);

static void pp_list(FILE *fp, int d, list_t list, string_t name, pp_func_t func)
{
    fprintf(fp, "%s(\n", name);
    for (; list; list = list->next)
        func(fp, d, list->data);
    indent(fp, d-1);
    fprintf(fp, ")\n");
}

void pp_decl(FILE *fp, int d, ast_decl_t decl)
{
    indent(fp, d);
    switch (decl->kind)
    {
        case AST_FUNCS_DECL:
            pp_list(fp, d+1, decl->u.funcs, "funcs_decl", (pp_func_t) pp_func);
            break;
        case AST_TYPES_DECL:
            pp_list(fp, d+1, decl->u.types, "types_decl", (pp_func_t) pp_nametype);
            break;
        case AST_VAR_DECL:
            fprintf(fp, "var_decl(%s\n", sym_name(decl->u.var.var));
            if (decl->u.var.type)
            {
                indent(fp, d+1);
                fprintf(fp, "%s\n", sym_name(decl->u.var.type));
            }
            pp_expr(fp, d+1, decl->u.var.init);
            indent(fp, d+1);
            fprintf(fp, "%s\n", decl->u.var.escape ? "TRUE" : "FALSE");
            indent(fp, d);
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
            fprintf(fp, "nil_expr()\n");
            break;
        case AST_VAR_EXPR:
            fprintf(fp, "var_expr(\n");
            pp_var(fp, d+1, expr->u.var);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_NUM_EXPR:
            fprintf(fp, "int_expr(%d)\n", expr->u.num);
            break;
        case AST_STRING_EXPR:
            fprintf(fp, "string_expr(%s)\n", expr->u.str);
            break;
        case AST_CALL_EXPR:
            fprintf(fp, "call_expr(%s\n", sym_name(expr->u.call.func));
            indent(fp, d+1);
            pp_list(fp, d+2, expr->u.call.args, "call_args", (pp_func_t) pp_expr);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_OP_EXPR:
            fprintf(fp, "op_expr(\n");
            indent(fp, d+1);
            pp_op(fp, expr->u.op.op);
            pp_expr(fp, d+1, expr->u.op.left);
            pp_expr(fp, d+1, expr->u.op.right);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_RECORD_EXPR:
            fprintf(fp, "record_expr(%s\n", sym_name(expr->u.record.type));
            indent(fp, d+1);
            pp_list(fp, d+2, expr->u.record.efields, "efields", (pp_func_t) pp_efield);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_ARRAY_EXPR:
            fprintf(fp, "array_expr(%s\n", sym_name(expr->u.array.type));
            pp_expr(fp, d+1, expr->u.array.size);
            pp_expr(fp, d+1, expr->u.array.init);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_SEQ_EXPR:
            pp_list(fp, d+1, expr->u.seq, "seq_exp", (pp_func_t) pp_expr);
            break;
        case AST_IF_EXPR:
            fprintf(fp, "if_expr(\n");
            pp_expr(fp, d+1, expr->u.if_.cond);
            pp_expr(fp, d+1, expr->u.if_.then);
            if (expr->u.if_.else_)
            {
                pp_expr(fp, d+1, expr->u.if_.else_);
            }
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_WHILE_EXPR:
            fprintf(fp, "while_expr(\n");
            pp_expr(fp, d+1, expr->u.while_.cond);
            pp_expr(fp, d+1, expr->u.while_.body);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_FOR_EXPR:
            fprintf(fp, "for_expr(%s,\n", sym_name(expr->u.for_.var));
            indent(fp, d+1);
            fprintf(fp, "%s\n", expr->u.for_.escape ? "TRUE" : "FALSE");
            pp_expr(fp, d+1, expr->u.for_.lo);
            pp_expr(fp, d+1, expr->u.for_.hi);
            pp_expr(fp, d+1, expr->u.for_.body);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_BREAK_EXPR:
            fprintf(fp, "break_expr()\n");
            break;
        case AST_LET_EXPR:
            fprintf(fp, "let_expr(\n");
            indent(fp, d+1);
            pp_list(fp, d+2, expr->u.let.decls, "decls", (pp_func_t) pp_decl);
            pp_expr(fp, d+1, expr->u.let.body);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_ASSIGN_EXPR:
            fprintf(fp, "assign_expr(\n");
            pp_var(fp, d+1, expr->u.assign.var);
            pp_expr(fp, d+1, expr->u.assign.expr);
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        default:
            assert(0);
    }
}

void pp_type(FILE *fp, int d, ast_type_t type)
{
    indent(fp, d);
    switch (type->kind)
    {
        case AST_NAME_TYPE:
            fprintf(fp, "name_type(%s)\n", sym_name(type->u.name));
            break;
        case AST_RECORD_TYPE:
            pp_list(fp, d+1, type->u.record, "record_type", (pp_func_t) pp_field);
            break;
        case AST_ARRAY_TYPE:
            fprintf(fp, "array_type(%s)\n", sym_name(type->u.array));
            break;
        default:
            assert(0);
    }
}

void pp_var(FILE *fp, int d, ast_var_t var)
{
    indent(fp, d);
    switch (var->kind)
    {
        case AST_SIMPLE_VAR:
            fprintf(fp, "simple_var(%s)\n", sym_name(var->u.simple));
            break;
        case AST_FIELD_VAR:
            fprintf(fp, "field_var(\n");
            pp_var(fp, d+1, var->u.field.var);
            indent(fp, d+1);
            fprintf(fp, "%s\n", sym_name(var->u.field.field));
            indent(fp, d);
            fprintf(fp, ")\n");
            break;
        case AST_SUB_VAR:
            fprintf(fp, "sub_var(\n");
            pp_var(fp, d+1, var->u.sub.var);
            pp_expr(fp, d+1, var->u.sub.sub);
            indent(fp, d);
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
        fprintf(fp, "efield(%s\n", sym_name(efield->name));
        pp_expr(fp, d+1, efield->expr);
        indent(fp, d);
        fprintf(fp, ")\n");
    }
    else
        fprintf(fp, "efield()\n");
}

static void pp_field(FILE *fp, int d, ast_field_t field)
{
    indent(fp, d);
    fprintf(fp, "field(%s\n", sym_name(field->name));
    indent(fp, d+1);
    fprintf(fp, "%s\n", sym_name(field->type));
    indent(fp, d+1);
    fprintf(fp, "%s\n", field->escape ? "TRUE" : "FALSE");
    indent(fp, d);
    fprintf(fp, ")\n");
}

static void pp_func(FILE *fp, int d, ast_func_t func)
{
    indent(fp, d);
    fprintf(fp, "func(%s\n", sym_name(func->name));
    indent(fp, d+1);
    pp_list(fp, d+2, func->params, "params", (pp_func_t) pp_field);
    if (func->result)
    {
        indent(fp, d+1);
        fprintf(fp, "%s\n", sym_name(func->result));
    }
    pp_expr(fp, d+1, func->body);
    indent(fp, d);
    fprintf(fp, ")\n");
}

static void pp_nametype(FILE *fp, int d, ast_nametype_t nametype)
{
    indent(fp, d);
    fprintf(fp, "nametype(%s\n", sym_name(nametype->name));
    pp_type(fp, d+1, nametype->type);
    indent(fp, d);
    fprintf(fp, ")\n");
}
