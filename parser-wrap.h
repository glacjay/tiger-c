#ifndef INCLUDE__PARSER_WRAP_H
#define INCLUDE__PARSER_WRAP_H

#include "ast.h"
#include "parser.h"
#include "symbol.h"
#include "utils.h"

extern int yydebug;

ast_expr_t parse(string_t filename);

#endif
