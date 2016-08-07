#include "ppir.h"

#include <assert.h>

#include "ir.h"
#include "temp.h"

static void pp_expr(FILE *out, int d, ir_expr_t expr);

static void indent(FILE *out, int d)
{
    int i;

    for (i = 0; i <= d; ++i)
    {
        fprintf(out, "    ");
    }
}

static char const* binops[] = {
    "PLUS",
    "MINUS",
    "TIMES",
    "DIVIDE",
    "AND",
    "OR",
    "XOR",
    "LSHIFT",
    "RSHIFT",
    "ARSHIFT",
};

static char const* relops[] = {
    "EQ",
    "NE",
    "LT",
    "LE",
    "GT",
    "GE",
    "ULT",
    "ULE",
    "UGT",
    "UGE",
};

static void pp_stmt(FILE *out, int d, ir_stmt_t stmt)
{
    switch (stmt->kind)
    {
        case IR_SEQ:
        {
            list_t p;

            indent(out, d);
            fprintf(out, "SEQ(\n");
            for (p = stmt->u.seq; p; p = p->next)
            {
                pp_stmt(out, d + 1, p->data);
            }
            indent(out, d);
            fprintf(out, ")\n");
            break;
        }

        case IR_LABEL:
            indent(out, d);
            fprintf(out, "LABEL %s\n", tmp_name(stmt->u.label));
            break;

        case IR_JUMP:
            indent(out, d);
            fprintf(out, "JUMP(\n");
            pp_expr(out, d + 1, stmt->u.jump.expr);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        case IR_CJUMP:
            indent(out, d);
            fprintf(out, "CJUMP(%s\n", relops[stmt->u.cjump.op]);
            pp_expr(out, d + 1, stmt->u.cjump.left);
            pp_expr(out, d + 1, stmt->u.cjump.right);
            indent(out, d + 1);
            fprintf(out, "%s, %s)\n",
                    tmp_name(stmt->u.cjump.t),
                    tmp_name(stmt->u.cjump.f));
            break;

        case IR_MOVE:
            indent(out, d);
            fprintf(out, "MOVE(\n");
            pp_expr(out, d + 1, stmt->u.move.dst);
            pp_expr(out, d + 1, stmt->u.move.src);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        case IR_EXPR:
            indent(out, d);
            fprintf(out, "EXPR(\n");
            pp_expr(out, d + 1, stmt->u.expr);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        default:
            assert(false);
    }
}

void pp_expr(FILE *out, int d, ir_expr_t expr)
{
    switch (expr->kind)
    {
        case IR_BINOP:
            indent(out, d);
            fprintf(out, "BINOP(%s\n", binops[expr->u.binop.op]);
            pp_expr(out, d + 1, expr->u.binop.left);
            pp_expr(out, d + 1, expr->u.binop.right);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        case IR_MEM:
            indent(out, d);
            fprintf(out, "MEM(\n");
            pp_expr(out, d + 1, expr->u.mem);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        case IR_TMP:
            indent(out, d);
            fprintf(out, "TEMP t%s\n", tmp_lookup(tmp_map(), expr->u.tmp));
            break;

        case IR_ESEQ:
            indent(out, d);
            fprintf(out, "ESEQ(\n");
            pp_stmt(out, d + 1, expr->u.eseq.stmt);
            pp_expr(out, d + 1, expr->u.eseq.expr);
            indent(out, d);
            fprintf(out, ")\n");
            break;

        case IR_NAME:
            indent(out, d);
            fprintf(out, "NAME %s\n", tmp_name(expr->u.name));
            break;

        case IR_CONST:
            indent(out, d);
            fprintf(out, "CONST %d\n", expr->u.const_);
            break;

        case IR_CALL:
        {
            list_t p;

            indent(out, d);
            fprintf(out, "CALL(\n");
            pp_expr(out, d + 1, expr->u.call.func);
            for (p = expr->u.call.args; p; p = p->next)
            {
                pp_expr(out, d + 1, p->data);
            }
            indent(out, d);
            fprintf(out, ")\n");
            break;
        }

        default:
            assert(false);
    }
}

void pp_stmts(FILE *out, list_t stmts)
{
    for (; stmts; stmts = stmts->next)
    {
        pp_stmt(out, 0, stmts->data);
    }
}
