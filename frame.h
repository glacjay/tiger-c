#ifndef INCLUDE__FRAME_H
#define INCLUDE__FRAME_H

#include "ir.h"
#include "temp.h"
#include "utils.h"

typedef struct frame_s *frame_t;
typedef struct fr_access_s *fr_access_t;

frame_t frame(tmp_label_t name, list_t formals);
tmp_label_t fr_name(frame_t fr);
list_t fr_formals(frame_t fr);
fr_access_t fr_alloc_local(frame_t fr, bool escape);
int fr_offset(fr_access_t access);

typedef struct fr_frag_s *fr_frag_t;
struct fr_frag_s
{
    enum { FR_STRING_FRAG, FR_PROC_FRAG, } kind;
    union
    {
        struct { tmp_label_t label; string_t string; } string;
        struct { ir_stmt_t stmt; frame_t frame; } proc;
    } u;
};
fr_frag_t fr_string_frag(tmp_label_t label, string_t string);
fr_frag_t fr_proc_frag(ir_stmt_t stmt, frame_t frame);
void fr_add_frag(fr_frag_t frag);

extern const int FR_WORD_SIZE;
temp_t fr_fp(void);
temp_t fr_rv(void);

ir_expr_t fr_expr(fr_access_t access, ir_expr_t frame_ptr);
ir_expr_t fr_external_call(string_t name, list_t args);

ir_stmt_t fr_proc_entry_exit_1(frame_t fr, ir_stmt_t stmt);

void fr_pp_frags(FILE *out);

#endif
