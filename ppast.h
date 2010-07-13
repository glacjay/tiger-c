#ifndef INCLUDE__PPAST_H
#define INCLUDE__PPAST_H

#include <stdio.h>

#include "ast.h"

void pp_decl(FILE *fp, int d, ast_decl_t decl);
void pp_expr(FILE *fp, int d, ast_expr_t expr);
void pp_type(FILE *fp, int d, ast_type_t type);
void pp_var(FILE *fp, int d, ast_var_t var);

#endif
