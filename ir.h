#ifndef INCLUDE__IR_H
#define INCLUDE__IR_H

#include "temp.h"
#include "utils.h"

typedef struct ir_stmt_s *ir_stmt_t;
typedef struct ir_expr_s *ir_expr_t;

typedef enum {
    IR_PLUS, IR_MINUS, IR_MUL, IR_DIV,
    IR_AND, IR_OR, IR_XOR,
    IR_LSHIFT, IR_RSHIFT, IR_ARSHIFT,
} ir_binop_t;

typedef enum {
    IR_EQ, IR_NE, IR_LT, IR_LE, IR_GT, IR_GE,
    IR_ULT, IR_ULE, IR_UGT, IR_UGE,
} ir_relop_t;

struct ir_stmt_s
{
    enum { IR_SEQ, IR_LABEL, IR_JUMP, IR_CJUMP, IR_MOVE, IR_EXPR } kind;
    union
    {
        list_t seq;
        tmp_label_t label;
        struct { ir_expr_t expr; list_t jumps; } jump;
        struct { ir_relop_t op; ir_expr_t left, right; tmp_label_t t, f; } cjump;
        struct { ir_expr_t dst, src; } move;
        ir_expr_t expr;
    } u;
};
ir_stmt_t ir_seq_stmt(list_t seq);
ir_stmt_t ir_label_stmt(tmp_label_t label);
ir_stmt_t ir_jump_stmt(ir_expr_t expr, list_t jumps);
ir_stmt_t ir_cjump_stmt(ir_relop_t op,
                        ir_expr_t left,
                        ir_expr_t right,
                        tmp_label_t t,
                        tmp_label_t f);
ir_stmt_t ir_move_stmt(ir_expr_t dst, ir_expr_t src);
ir_stmt_t ir_expr_stmt(ir_expr_t expr);

struct ir_expr_s
{
    enum { IR_BINOP, IR_MEM, IR_TMP, IR_ESEQ, IR_NAME, IR_CONST, IR_CALL } kind;
    union
    {
        struct { ir_binop_t op; ir_expr_t left, right; } binop;
        ir_expr_t mem;
        temp_t tmp;
        struct { ir_stmt_t stmt; ir_expr_t expr; } eseq;
        tmp_label_t name;
        int const_;
        struct { ir_expr_t func; list_t args; } call;
    } u;
};
ir_expr_t ir_binop_expr(ir_binop_t op, ir_expr_t left, ir_expr_t right);
ir_expr_t ir_mem_expr(ir_expr_t mem);
ir_expr_t ir_tmp_expr(temp_t tmp);
ir_expr_t ir_eseq_expr(ir_stmt_t stmt, ir_expr_t expr);
ir_expr_t ir_name_expr(tmp_label_t name);
ir_expr_t ir_const_expr(int const_);
ir_expr_t ir_call_expr(ir_expr_t func, list_t args);

#endif
