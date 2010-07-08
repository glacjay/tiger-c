#ifndef INCLUDE__AST_H
#define INCLUDE__AST_H

#include "symbol.h"
#include "utils.h"

typedef struct ast_decl_s *ast_decl_t;
typedef struct ast_expr_s *ast_expr_t;
typedef struct ast_type_s *ast_type_t;
typedef struct ast_var_s *ast_var_t;

typedef struct ast_efield_s *ast_efield_t;
typedef struct ast_field_s *ast_field_t;
typedef struct ast_func_s *ast_func_t;
typedef struct ast_nametype_s *ast_nametype_t;

typedef enum ast_binop_e ast_binop_t;
enum ast_binop_e
{
    AST_PLUS, AST_MINUS, AST_TIMES, AST_DIVIDE,
    AST_EQ, AST_NEQ, AST_LT, AST_LE, AST_GT, AST_GE,
    AST_AND, AST_OR,
};

struct ast_decl_s
{
    enum { AST_FUNCS_DECL, AST_TYPES_DECL, AST_VAR_DECL, } kind;
    union
    {
        list_t funcs;
        list_t types;
        struct { symbol_t var; symbol_t type; ast_expr_t init; } var;
    } u;
};
ast_decl_t ast_funcs_decl(list_t funcs);
ast_decl_t ast_types_decl(list_t types);
ast_decl_t ast_var_decl(symbol_t var, symbol_t type, ast_expr_t init);

struct ast_expr_s
{
    enum
    {
        AST_NIL_EXPR, AST_VAR_EXPR, AST_NUM_EXPR, AST_STRING_EXPR,
        AST_CALL_EXPR, AST_OP_EXPR, AST_RECORD_EXPR, AST_ARRAY_EXPR,
        AST_SEQ_EXPR, AST_IF_EXPR, AST_WHILE_EXPR, AST_FOR_EXPR,
        AST_BREAK_EXPR, AST_LET_EXPR, AST_ASSIGN_EXPR,
    } kind;
    union
    {
        /* nil; */
        ast_var_t var;
        int num;
        string_t str;
        struct { symbol_t func; list_t args; } call;
        struct { ast_expr_t left; ast_binop_t op; ast_expr_t right; } op;
        struct { symbol_t type; list_t efields; } record;
        struct { symbol_t type; ast_expr_t size, init; } array;
        list_t seq;
        struct { ast_expr_t cond, then, else_; } if_;
        struct { ast_expr_t cond, body; } while_;
        struct { symbol_t var; ast_expr_t lo, hi, body; } for_;
        /* break; */
        struct { list_t decls; ast_expr_t body; } let;
        struct { ast_var_t var; ast_expr_t expr; } assign;
    } u;
};
ast_expr_t ast_nil_expr(void);
ast_expr_t ast_var_expr(ast_var_t var);
ast_expr_t ast_num_expr(int num);
ast_expr_t ast_string_expr(string_t str);
ast_expr_t ast_call_expr(symbol_t func, list_t args);
ast_expr_t ast_op_expr(ast_expr_t left, ast_binop_t op, ast_expr_t right);
ast_expr_t ast_record_expr(symbol_t type, list_t efields);
ast_expr_t ast_array_expr(symbol_t type, ast_expr_t size, ast_expr_t init);
ast_expr_t ast_seq_expr(list_t seq);
ast_expr_t ast_if_expr(ast_expr_t cond, ast_expr_t then, ast_expr_t else_);
ast_expr_t ast_while_expr(ast_expr_t cond, ast_expr_t body);
ast_expr_t ast_for_expr(symbol_t var, ast_expr_t lo, ast_expr_t hi, ast_expr_t body);
ast_expr_t ast_break_expr(void);
ast_expr_t ast_let_expr(list_t decls, ast_expr_t body);
ast_expr_t ast_assign_expr(ast_var_t var, ast_expr_t expr);

struct ast_type_s
{
    enum { AST_NAME_TYPE, AST_RECORD_TYPE, AST_ARRAY_TYPE } kind;
    union
    {
        symbol_t name;
        list_t record;
        symbol_t array;
    } u;
};
ast_type_t ast_name_type(symbol_t name);
ast_type_t ast_record_type(list_t record);
ast_type_t ast_array_type(symbol_t array);

struct ast_var_s
{
    enum { AST_SIMPLE_VAR, AST_FIELD_VAR, AST_SUB_VAR } kind;
    union
    {
        symbol_t simple;
        /* var must be the first field to simplify the parser implementation. */
        struct { ast_var_t var; symbol_t field; } field;
        struct { ast_var_t var; ast_expr_t sub; } sub;
    } u;
};
ast_var_t ast_simple_var(symbol_t simple);
ast_var_t ast_field_var(ast_var_t var, symbol_t field);
ast_var_t ast_sub_var(ast_var_t var, ast_expr_t sub);

struct ast_efield_s { symbol_t name; ast_expr_t expr; };
ast_efield_t ast_efield(symbol_t name, ast_expr_t expr);
struct ast_field_s { symbol_t name, type; };
ast_field_t ast_field(symbol_t name, symbol_t type);
struct ast_func_s { symbol_t name; list_t params; symbol_t result; ast_expr_t body; };
ast_func_t ast_func(symbol_t name, list_t params, symbol_t result, ast_expr_t body);
struct ast_nametype_s { symbol_t name; ast_type_t type; };
ast_nametype_t ast_nametype(symbol_t name, ast_type_t type);

#endif
