#ifndef INCLUDE__PPAST_H
#define INCLUDE__PPAST_H

#include <stdio.h>

#include "ast.h"

void pp_expr(FILE *fp, int d, ast_expr_t expr);

#endif
