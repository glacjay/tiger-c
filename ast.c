#include "ast.h"

ast_stmt_t ast_compound_stmt(ast_stmt_t stmt1, ast_stmt_t stmt2)
{
    ast_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = AST_COMPOUND_STMT;
    p->u.compound.stmt1 = stmt1;
    p->u.compound.stmt2 = stmt2;
    return p;
}

ast_stmt_t ast_assign_stmt(string_t id, ast_expr_t expr)
{
    ast_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ASSIGN_STMT;
    p->u.assign.id = id;
    p->u.assign.expr = expr;
    return p;
}

ast_stmt_t ast_print_stmt(ast_expr_list_t exprs)
{
    ast_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = AST_PRINT_STMT;
    p->u.print.exprs = exprs;
    return p;
}

ast_expr_t ast_id_expr(string_t id)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ID_EXPR;
    p->u.id = id;
    return p;
}

ast_expr_t ast_num_expr(int num)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_NUM_EXPR;
    p->u.num = num;
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

ast_expr_t ast_eseq_expr(ast_stmt_t stmt, ast_expr_t expr)
{
    ast_expr_t p = checked_malloc(sizeof(*p));
    p->kind = AST_ESEQ_EXPR;
    p->u.eseq.stmt = stmt;
    p->u.eseq.expr = expr;
    return p;
}

ast_expr_list_t ast_pair_expr_list(ast_expr_t head, ast_expr_list_t tail)
{
    ast_expr_list_t p = checked_malloc(sizeof(*p));
    p->kind = AST_PAIR_EXPR_LIST;
    p->u.pair.head = head;
    p->u.pair.tail = tail;
    return p;
}

ast_expr_list_t ast_last_expr_list(ast_expr_t last)
{
    ast_expr_list_t p = checked_malloc(sizeof(*p));
    p->kind = AST_LAST_EXPR_LIST;
    p->u.last = last;
    return p;
}
