#ifndef INCLUDE__TRANSLATE_H
#define INCLUDE__TRANSLATE_H

#include "ir.h"
#include "temp.h"
#include "utils.h"

typedef struct tr_access_s *tr_access_t;
typedef struct tr_level_s *tr_level_t;

tr_level_t tr_outermost(void);
tr_level_t tr_level(tr_level_t parent, tmp_label_t name, list_t formals);
list_t tr_formals(tr_level_t level);
tr_access_t tr_alloc_local(tr_level_t level, bool escape);

typedef struct tr_expr_s *tr_expr_t;
tr_expr_t tr_ex(ir_expr_t expr);
tr_expr_t tr_nx(ir_stmt_t stmt);
tr_expr_t tr_cx(list_t trues, list_t falses, ir_stmt_t stmt);

tr_expr_t tr_simple_var(tr_access_t access, tr_level_t level);

#endif
