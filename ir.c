#include "ir.h"

ir_stmt_t ir_seq_stmt(list_t seq)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_SEQ;
    p->u.seq = seq;
    return p;
}

ir_stmt_t ir_label_stmt(tmp_label_t label)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_LABEL;
    p->u.label = label;
    return p;
}

ir_stmt_t ir_jump_stmt(ir_expr_t expr, list_t jumps)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_JUMP;
    p->u.jump.expr = expr;
    p->u.jump.jumps = jumps;
    return p;
}

ir_stmt_t ir_cjump_stmt(ir_relop_t op,
                        ir_expr_t left,
                        ir_expr_t right,
                        tmp_label_t t,
                        tmp_label_t f)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_CJUMP;
    p->u.cjump.op = op;
    p->u.cjump.left = left;
    p->u.cjump.right = right;
    p->u.cjump.t = t;
    p->u.cjump.f = f;
    return p;
}

ir_stmt_t ir_move_stmt(ir_expr_t dst, ir_expr_t src)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_MOVE;
    p->u.move.dst = dst;
    p->u.move.src = src;
    return p;
}

ir_stmt_t ir_expr_stmt(ir_expr_t expr)
{
    ir_stmt_t p = checked_malloc(sizeof(*p));
    p->kind = IR_EXPR;
    p->u.expr = expr;
    return p;
}

ir_expr_t ir_binop_expr(ir_binop_t op, ir_expr_t left, ir_expr_t right)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_BINOP;
    p->u.binop.op = op;
    p->u.binop.left = left;
    p->u.binop.right = right;
    return p;
}

ir_expr_t ir_mem_expr(ir_expr_t mem)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_MEM;
    p->u.mem = mem;
    return p;
}

ir_expr_t ir_tmp_expr(temp_t tmp)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_TMP;
    p->u.tmp = tmp;
    return p;
}

ir_expr_t ir_eseq_expr(ir_stmt_t stmt, ir_expr_t expr)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_ESEQ;
    p->u.eseq.stmt = stmt;
    p->u.eseq.expr = expr;
    return p;
}

ir_expr_t ir_name_expr(tmp_label_t name)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_NAME;
    p->u.name = name;
    return p;
}

ir_expr_t ir_const_expr(int const_)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_CONST;
    p->u.const_ = const_;
    return p;
}

ir_expr_t ir_call_expr(ir_expr_t func, list_t args)
{
    ir_expr_t p = checked_malloc(sizeof(*p));
    p->kind = IR_CALL;
    p->u.call.func = func;
    p->u.call.args = args;
    return p;
}
