#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "utils.h"

typedef struct table_s {
    string_t id;
    int value;
    struct table_s *next;
} *table_t;

typedef struct int_and_table_s {
    int i;
    table_t t;
} int_and_table_t;

static table_t table(string_t id, int value, table_t next)
{
    table_t p = checked_malloc(sizeof(*p));
    p->id = id;
    p->value = value;
    p->next = next;
    return p;
}

static table_t lookup(table_t t, string_t id)
{
    if (!t)
        return NULL;
    if (strcmp(t->id, id) == 0)
        return t;
    return lookup(t->next, id);
}

static int_and_table_t interp_expr(ast_expr_t expr, table_t t);
static table_t interp_stmt(ast_stmt_t stmt, table_t t);

static int_and_table_t interp_op_expr(ast_expr_t expr, table_t t)
{
    int_and_table_t temp = interp_expr(expr->u.op.left, t);
    int_and_table_t result = interp_expr(expr->u.op.right, temp.t);

    switch (expr->u.op.op) {
        case AST_PLUS:
            result.i = temp.i + result.i;
            break;
        case AST_MINUS:
            result.i = temp.i - result.i;
            break;
        case AST_TIMES:
            result.i = temp.i * result.i;
            break;
        case AST_DIV:
            result.i = temp.i / result.i;
            break;
        default:
            fprintf(stderr, "Wrong binary operator: %d\n", expr->u.op.op);
            exit(1);
    }
    return result;
}

static int_and_table_t interp_expr(ast_expr_t expr, table_t t)
{
    int_and_table_t result;

    switch (expr->kind) {
        case AST_ID_EXPR: {
            table_t entry = lookup(t, expr->u.id);
            if (!entry) {
                fprintf(stderr, "Undefined variable: %s\n", expr->u.id);
                exit(1);
            }
            result.i = entry->value;
            result.t = t;
            return result;
        }
        case AST_NUM_EXPR:
            result.i = expr->u.num;
            result.t = t;
            return result;
        case AST_OP_EXPR:
            return interp_op_expr(expr, t);
        case AST_ESEQ_EXPR:
            t = interp_stmt(expr->u.eseq.stmt, t);
            return interp_expr(expr->u.eseq.expr, t);
        default:
            fprintf(stderr, "Wrong expression kind: %d\n", expr->kind);
            exit(1);
    }
}

static table_t interp_print_stmt(ast_expr_list_t list, table_t t)
{
    int_and_table_t result;

    for (; list->kind == AST_PAIR_EXPR_LIST; list = list->u.pair.tail) {
        result = interp_expr(list->u.pair.head, t);
        printf("%d ", result.i);
        t = result.t;
    }
    if (list->kind != AST_LAST_EXPR_LIST) {
        fprintf(stderr, "Wrong expr_list kind: %d\n", list->kind);
        exit(1);
    }
    result = interp_expr(list->u.last, t);
    printf("%d ", result.i);
    return result.t;
}

static table_t interp_stmt(ast_stmt_t stmt, table_t t)
{
    switch (stmt->kind) {
        case AST_COMPOUND_STMT:
            t = interp_stmt(stmt->u.compound.stmt1, t);
            return interp_stmt(stmt->u.compound.stmt2, t);
        case AST_ASSIGN_STMT: {
            int_and_table_t result = interp_expr(stmt->u.assign.expr, t);
            return table(stmt->u.assign.id, result.i, result.t);
        }
        case AST_PRINT_STMT:
            return interp_print_stmt(stmt->u.print.exprs, t);
        default:
            fprintf(stderr, "Wrong statement kind: %d\n", stmt->kind);
            exit(1);
    }
}

static void interp(ast_stmt_t stmt)
{
    interp_stmt(stmt, NULL);
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
    interp(prog);
    printf("\n");

    return 0;
}
