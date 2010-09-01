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

tr_expr_t tr_num_expr(int num);
tr_expr_t tr_string_expr(string_t str);
tr_expr_t tr_call_expr(tr_level_t level, tmp_label_t label, list_t args);
tr_expr_t tr_op_expr(int op, tr_expr_t left, tr_expr_t right);
tr_expr_t tr_rel_expr(int op, tr_expr_t left, tr_expr_t right);
tr_expr_t tr_string_rel_expr(int op, tr_expr_t left, tr_expr_t right);
tr_expr_t tr_record_expr(list_t fields, int size);
tr_expr_t tr_array_expr(tr_expr_t size, tr_expr_t init);
tr_expr_t tr_seq_expr(list_t stmts);
tr_expr_t tr_if_expr(tr_expr_t cond, tr_expr_t then, tr_expr_t else_);
tr_expr_t tr_while_expr(tr_expr_t cond, tr_expr_t body);
tr_expr_t tr_for_expr(tr_access_t access,
                      tr_expr_t low,
                      tr_expr_t high,
                      tr_expr_t body);
tr_expr_t tr_assign_expr(tr_expr_t lhs, tr_expr_t rhs);

tr_expr_t tr_simple_var(tr_access_t access, tr_level_t level);

#endif
