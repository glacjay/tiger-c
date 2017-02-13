#include "frame.h"

#define K 4
const int FR_WORD_SIZE = 4;

struct frame_s
{
    tmp_label_t name;
    list_t formals;
    list_t locals;
    int local_count;
};

struct fr_access_s
{
    enum { FR_IN_FRAME, FR_IN_REG } kind;
    union
    {
        int offset;
        temp_t reg;
    } u;
};

static fr_access_t in_frame(int offset)
{
    fr_access_t p = checked_malloc(sizeof(*p));
    p->kind = FR_IN_FRAME;
    p->u.offset = offset;
    return p;
}

static fr_access_t in_reg(temp_t reg)
{
    fr_access_t p = checked_malloc(sizeof(*p));
    p->kind = FR_IN_REG;
    p->u.reg = reg;
    return p;
}

frame_t frame(tmp_label_t name, list_t formals)
{
    frame_t p = checked_malloc(sizeof(*p));
    list_t formal = formals, q = NULL;
    int i = 0;

    p->name = name;
    p->locals = NULL;
    p->local_count = 0;
    for (; formal; formal = formal->next, i++)
    {
        fr_access_t access;
        if (formal->b || i >= K)
            access = in_frame(i * FR_WORD_SIZE);
        else
            access = in_reg(temp());
        if (q)
        {
            q->next = list(access, NULL);
            q = q->next;
        }
        else
            p->formals = q = list(access, NULL);
    }
    return p;
}

tmp_label_t fr_name(frame_t fr)
{
    return fr->name;
}

list_t fr_formals(frame_t fr)
{
    return fr->formals;
}

fr_access_t fr_alloc_local(frame_t fr, bool escape)
{
    fr_access_t access;

    if (escape)
    {
        fr->local_count++;
        /* -2 for the the return address and frame pointer. */
        access = in_frame(FR_WORD_SIZE * (-2 - fr->local_count));
    }
    else
        access = in_reg(temp());
    if (fr->locals)
    {
        list_t p = fr->locals;
        while (p->next)
            p = p->next;
        p->next = list(access, NULL);
    }
    else
        fr->locals = list(access, NULL);
    return access;
}

int fr_offset(fr_access_t access)
{
    assert(access && access->kind == FR_IN_FRAME);
    return access->u.offset;
}

fr_frag_t fr_string_frag(tmp_label_t label, string_t string)
{
    fr_frag_t p = checked_malloc(sizeof(*p));
    p->kind = FR_STRING_FRAG;
    p->u.string.label = label;
    p->u.string.string = string;
    return p;
}

fr_frag_t fr_proc_frag(ir_stmt_t stmt, frame_t frame)
{
    fr_frag_t p = checked_malloc(sizeof(*p));
    p->kind = FR_PROC_FRAG;
    p->u.proc.stmt = stmt;
    p->u.proc.frame = frame;
    return p;
}

static list_t _string_frags = NULL;
static list_t _proc_frags = NULL;

void fr_add_frag(fr_frag_t frag)
{
    switch (frag->kind)
    {
        case FR_STRING_FRAG:
            _string_frags = list_append(_string_frags, frag);
            break;
        case FR_PROC_FRAG:
            _proc_frags = list_append(_proc_frags, frag);
            break;
        default:
            assert(false);
    }
}

temp_t fr_fp(void)
{
    static temp_t _fp = NULL;

    if (!_fp)
        _fp = temp();
    return _fp;
}

temp_t fr_rv(void)
{
    static temp_t _rv = NULL;

    if (!_rv)
    {
        _rv = temp();
    }
    return _rv;
}

ir_expr_t fr_expr(fr_access_t access, ir_expr_t frame_ptr)
{
    switch (access->kind)
    {
        case FR_IN_FRAME:
            return ir_mem_expr(ir_binop_expr(
                IR_PLUS,
                ir_const_expr(access->u.offset),
                frame_ptr));

        case FR_IN_REG:
            return ir_tmp_expr(access->u.reg);
    }

    assert(0);
    return NULL;
}

ir_expr_t fr_external_call(string_t name, list_t args)
{
    return ir_call_expr(ir_name_expr(tmp_named_label(name)), args);
}

void fr_pp_frags(FILE *out)
{
    list_t p;

    fprintf(out, "STRING FRAGMENTS:\n");
    for (p = _string_frags; p; p = p->next)
    {
        fr_frag_t frag = p->data;
        fprintf(out, "    %s: \"%s\"\n",
                tmp_name(frag->u.string.label),
                frag->u.string.string);
    }
    fprintf(out, "\n");

    fprintf(out, "FUNCTION FRAGMENTS:\n");
    for (p = _proc_frags; p; p = p->next)
    {
        fr_frag_t frag = p->data;
        fprintf(out, "    %s:\n", tmp_name(frag->u.proc.frame->name));
        fprintf(out, "\n");
    }
    fprintf(out, "\n");
}

ir_stmt_t fr_proc_entry_exit_1(frame_t fr, ir_stmt_t stmt)
{
    return stmt;
}
