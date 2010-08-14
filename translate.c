#include <assert.h>

#include "frame.h"
#include "ir.h"
#include "translate.h"

struct tr_access_s
{
    tr_level_t level;
    fr_access_t access;
};

static tr_access_t tr_access(tr_level_t level, fr_access_t access)
{
    tr_access_t p = checked_malloc(sizeof(*p));
    p->level = level;
    p->access = access;
    return p;
}

struct tr_level_s
{
    tr_level_t parent;
    frame_t frame;
    list_t formals;
    list_t locals;
};

static tr_level_t _outermost = NULL;

tr_level_t tr_outermost(void)
{
    if (!_outermost)
        _outermost = tr_level(NULL, tmp_label(), NULL);
    return _outermost;
}

tr_level_t tr_level(tr_level_t parent, tmp_label_t name, list_t formals)
{
    tr_level_t p = checked_malloc(sizeof(*p));
    list_t fr_formal, q = NULL;

    p->parent = parent;
    /* extra formal for static link */
    p->frame = frame(name, bool_list(true, formals));
    fr_formal = fr_formals(p->frame);
    for (; fr_formal; fr_formal = fr_formal->next)
    {
        tr_access_t access = tr_access(p, fr_formal->data);
        if (q)
        {
            q->next = list(access, NULL);
            q = q->next;
        }
        else
            p->formals = q = list(access, NULL);
    }
    p->locals = NULL;
    return p;
}

list_t tr_formals(tr_level_t level)
{
    return level->formals;
}

tr_access_t tr_alloc_local(tr_level_t level, bool escape)
{
    fr_access_t fr_access = fr_alloc_local(level->frame, escape);
    tr_access_t access = tr_access(level, fr_access);

    if (level->locals)
    {
        list_t p = level->locals;
        while (p->next)
            p = p->next;
        p->next = list(access, NULL);
    }
    else
        level->locals = list(access, NULL);
    return access;
}

static tr_access_t tr_static_link(tr_level_t level)
{
    assert(level);
    return level->formals->data;
}

typedef struct cx_s cx_t;
struct cx_s
{
    list_t trues;
    list_t falses;
    ir_stmt_t stmt;
};

struct tr_expr_s
{
    enum { TR_EX, TR_NX, TR_CX } kind;
    union
    {
        ir_expr_t ex;
        ir_stmt_t nx;
        cx_t cx;
    } u;
};

static tr_expr_t tr_ex(ir_expr_t expr)
{
    tr_expr_t p = checked_malloc(sizeof(*p));
    p->kind = TR_EX;
    p->u.ex = expr;
    return p;
}

static tr_expr_t tr_nx(ir_stmt_t stmt)
{
    tr_expr_t p = checked_malloc(sizeof(*p));
    p->kind = TR_NX;
    p->u.nx = stmt;
    return p;
}

static tr_expr_t tr_cx(list_t trues, list_t falses, ir_stmt_t stmt)
{
    tr_expr_t p = checked_malloc(sizeof(*p));
    p->kind = TR_CX;
    p->u.cx.trues = trues;
    p->u.cx.falses = falses;
    p->u.cx.stmt = stmt;
    return p;
}

static void fill_patch(list_t patchs, tmp_label_t label)
{
    list_t p = patchs;
    for (; p; p = p->next)
        *((tmp_label_t *) p->data) = label;
}

static ir_expr_t un_ex(tr_expr_t expr)
{
    switch (expr->kind)
    {
        case TR_EX:
            return expr->u.ex;
        case TR_NX:
            return ir_eseq_expr(expr->u.nx, ir_const_expr(0));
        case TR_CX: {
            temp_t tmp = temp();
            tmp_label_t t = tmp_label();
            tmp_label_t f = tmp_label();
            fill_patch(expr->u.cx.trues, t);
            fill_patch(expr->u.cx.falses, f);
            return ir_eseq_expr(
                    ir_seq_stmt(
                            list(ir_move_stmt(ir_tmp_expr(tmp),
                                              ir_const_expr(1)),
                                 list(expr->u.cx.stmt,
                                      list(ir_label_stmt(f),
                                           list(ir_move_stmt(ir_tmp_expr(tmp),
                                                             ir_const_expr(0)),
                                                list(ir_label_stmt(t),
                                                     NULL)))))),
                    ir_tmp_expr(tmp));
        }
    }

    assert(0);
}

static ir_stmt_t un_nx(tr_expr_t expr)
{
    switch (expr->kind)
    {
        case TR_EX:
            return ir_expr_stmt(expr->u.ex);
        case TR_NX:
            return expr->u.nx;
        case TR_CX: {
            tmp_label_t label = tmp_label();
            fill_patch(expr->u.cx.trues, label);
            fill_patch(expr->u.cx.falses, label);
            return ir_seq_stmt(list(expr->u.cx.stmt,
                                    list(ir_label_stmt(label), NULL)));
        }
    }

    assert(0);
}

static cx_t un_cx(tr_expr_t expr)
{
    switch (expr->kind)
    {
        case TR_EX: {
            cx_t cx;
            cx.stmt = ir_cjump_stmt(
                    IR_EQ, expr->u.ex, ir_const_expr(0), NULL, NULL);
            cx.trues = list(&(cx.stmt->u.cjump.t), NULL);
            cx.falses = list(&(cx.stmt->u.cjump.f), NULL);
            return cx;
        }
        case TR_NX:
            assert(0);
        case TR_CX:
            return expr->u.cx;
    }

    assert(0);
}

tr_expr_t tr_num_expr(int num)
{
    return tr_ex(ir_const_expr(num));
}

tr_expr_t tr_simple_var(tr_access_t access, tr_level_t level)
{
    ir_expr_t fp = ir_tmp_expr(fr_fp());

    while (level != access->level)
    {
        fr_access_t fr_access = tr_static_link(level)->access;
        fp = ir_mem_expr(ir_binop_expr(
                        IR_PLUS, fp,
                        ir_const_expr(fr_offset(fr_access))));
        level = level->parent;
    }
    return tr_ex(ir_mem_expr(ir_binop_expr(
                            IR_PLUS, fp,
                            ir_const_expr(fr_offset(access->access)))));
}
