#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

static int maxargs_stmt(ast_stmt_t stmt);

static int maxargs_expr(ast_expr_t expr)
{
    switch (expr->kind) {
        case AST_ID_EXPR:
        case AST_NUM_EXPR:
            return 0;
        case AST_OP_EXPR: {
            int max1 = maxargs_expr(expr->u.op.left);
            int max2 = maxargs_expr(expr->u.op.right);
            return max1 > max2 ? max1 : max2;
        }
        case AST_ESEQ_EXPR: {
            int max1 = maxargs_stmt(expr->u.eseq.stmt);
            int max2 = maxargs_expr(expr->u.eseq.expr);
            return max1 > max2 ? max1 : max2;
        }
        default:
            fprintf(stderr, "Wrong expression kind: %d\n", expr->kind);
            return 0;
    }
}

static int maxargs_expr_list(ast_expr_list_t exprs)
{
    switch (exprs->kind) {
        case AST_PAIR_EXPR_LIST:
            return 1 + maxargs_expr_list(exprs->u.pair.tail);
        case AST_LAST_EXPR_LIST:
            return 1;
        default:
            fprintf(stderr, "Wrong expr_list kind: %d\n", exprs->kind);
            return 0;
    }
}

static int maxargs_stmt(ast_stmt_t stmt)
{
    switch (stmt->kind) {
        case AST_COMPOUND_STMT: {
            int max1 = maxargs_stmt(stmt->u.compound.stmt1);
            int max2 = maxargs_stmt(stmt->u.compound.stmt2);
            return max1 > max2 ? max1 : max2;
        }
        case AST_ASSIGN_STMT:
            return maxargs_expr(stmt->u.assign.expr);
        case AST_PRINT_STMT:
            return maxargs_expr_list(stmt->u.print.exprs);
        default:
            fprintf(stderr, "Wrong statement kind: %d\n", stmt->kind);
            return 0;
    }
}

static int maxargs(ast_stmt_t stmt)
{
    return maxargs_stmt(stmt);
}

int main(void)
{
    ast_stmt_t prog =
            ast_compound_stmt(
                    ast_assign_stmt(
                            "a",
                            ast_op_expr(
                                    ast_num_expr(5),
                                    AST_PLUS,
                                    ast_num_expr(3))),
                    ast_compound_stmt(
                            ast_assign_stmt(
                                    "b",
                                    ast_eseq_expr(
                                            ast_print_stmt(
                                                    ast_pair_expr_list(
                                                            ast_id_expr("a"),
                                                            ast_last_expr_list(
                                                                    ast_op_expr(
                                                                            ast_id_expr("a"),
                                                                            AST_MINUS,
                                                                            ast_num_expr(1))))),
                                            ast_op_expr(
                                                    ast_num_expr(10),
                                                    AST_TIMES,
                                                    ast_id_expr("a")))),
                            ast_print_stmt(
                                    ast_last_expr_list(ast_id_expr("b")))));
    printf("maxargs of prog: %d\n", maxargs(prog));

    return 0;
}
