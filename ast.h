#ifndef INCLUDE__AST_H
#define INCLUDE__AST_H

#include "utils.h"

typedef struct ast_stmt_s *ast_stmt_t;
typedef struct ast_expr_s *ast_expr_t;
typedef struct ast_expr_list_s *ast_expr_list_t;

typedef enum { AST_PLUS, AST_MINUS, AST_TIMES, AST_DIV } ast_binop_t;

struct ast_stmt_s {
    enum { AST_COMPOUND_STMT, AST_ASSIGN_STMT, AST_PRINT_STMT } kind;
    union {
        struct { ast_stmt_t stmt1, stmt2; } compound;
        struct { string_t id; ast_expr_t expr; } assign;
        struct { ast_expr_list_t exprs; } print;
    } u;
};
ast_stmt_t ast_compound_stmt(ast_stmt_t stmt1, ast_stmt_t stmt2);
ast_stmt_t ast_assign_stmt(string_t id, ast_expr_t expr);
ast_stmt_t ast_print_stmt(ast_expr_list_t exprs);

struct ast_expr_s {
    enum { AST_ID_EXPR, AST_NUM_EXPR, AST_OP_EXPR, AST_ESEQ_EXPR } kind;
    union {
        string_t id;
        int num;
        struct { ast_expr_t left; ast_binop_t op; ast_expr_t right; } op;
        struct { ast_stmt_t stmt; ast_expr_t expr; } eseq;
    } u;
};
ast_expr_t ast_id_expr(string_t id);
ast_expr_t ast_num_expr(int num);
ast_expr_t ast_op_expr(ast_expr_t left, ast_binop_t op, ast_expr_t right);
ast_expr_t ast_eseq_expr(ast_stmt_t stmt, ast_expr_t expr);

struct ast_expr_list_s {
    enum { AST_PAIR_EXPR_LIST, AST_LAST_EXPR_LIST } kind;
    union {
        struct { ast_expr_t head; ast_expr_list_t tail; } pair;
        ast_expr_t last;
    } u;
};
ast_expr_list_t ast_pair_expr_list(ast_expr_t head, ast_expr_list_t tail);
ast_expr_list_t ast_last_expr_list(ast_expr_t last);

#endif
