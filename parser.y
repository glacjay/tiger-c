%{
#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"

int yylex(void);
void yyerror(char *msg);
%}

%union {
    int pos;
    int num;
    string_t str;
}

%token <str> TK_ID TK_STRING
%token <num> TK_INT

%token
    TK_COMMA TK_COLON TK_SEMICOLON TK_LPARAN TK_RPARAN TK_LBRACK TK_RBRACK
    TK_LBRACE TK_RBRACE TK_DOT
    TK_PLUS TK_MINUS TK_TIMES TK_DIVIDE TK_EQ TK_NEQ TK_LT TK_LE TK_GT TK_GE
    TK_AND TK_OR TK_ASSIGN
    TK_ARRAY TK_IF TK_THEN TK_ELSE TK_WHILE TK_FOR TK_TO TK_DO TK_LET TK_IN
    TK_END TK_OF TK_BREAK TK_NIL
    TK_FUNCTION TK_VAR TK_TYPE

%start program

%%

program:
    expr

expr:
    TK_ID

%%

void yyerror(char *msg)
{
    em_error(em_tok_pos, "%s", msg);
}
