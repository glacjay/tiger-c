%{
#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"

int yylex(void);
void yyerror(char *msg);
%}

%debug

%union {
    int pos;
    int num;
    string_t str;
}

%{
static void print_token_value(FILE *fp, int type, YYSTYPE value);
#define YYPRINT(fp, type, value) print_token_value(fp, type, value)
%}

%token <str> TK_ID TK_STRING
%token <num> TK_INT

%token <pos>
    TK_COMMA TK_COLON TK_SEMICOLON TK_LPARAN TK_RPARAN TK_LBRACK TK_RBRACK
    TK_LBRACE TK_RBRACE TK_DOT TK_ASSIGN
    TK_ARRAY TK_IF TK_THEN TK_ELSE TK_WHILE TK_FOR TK_TO TK_DO TK_LET TK_IN
    TK_END TK_OF TK_BREAK TK_NIL
    TK_FUNCTION TK_VAR TK_TYPE

%left <pos> TK_OR
%left <pos> TK_AND
%nonassoc <pos> TK_EQ TK_NEQ TK_LT TK_LE TK_GT TK_GE
%left <pos> TK_PLUS TK_MINUS
%left <pos> TK_TIMES TK_DIVIDE
%left <pos> TK_UMINUS

%start program

%%

program:
    expr

expr:
    lvalue
|   TK_NIL
|   expr expr_seq
|   TK_LPARAN TK_RPARAN
|   TK_LPARAN expr TK_RPARAN
|   TK_INT
|   TK_STRING
|   TK_MINUS expr %prec TK_UMINUS
|   id TK_LPARAN TK_RPARAN
|   id TK_LPARAN expr param_seq TK_RPARAN
|   expr TK_PLUS expr
|   expr TK_MINUS expr
|   expr TK_TIMES expr
|   expr TK_DIVIDE expr
|   expr TK_EQ expr
|   expr TK_NEQ expr
|   expr TK_LT expr
|   expr TK_LE expr
|   expr TK_GT expr
|   expr TK_GE expr
|   expr TK_AND expr
|   expr TK_OR expr
|   id TK_LBRACE TK_RBRACE
|   id TK_LBRACE id TK_EQ expr field_seq TK_RBRACE
|   id TK_LBRACK expr TK_RBRACK TK_OF expr
|   lvalue TK_ASSIGN expr
|   TK_IF expr TK_THEN expr
|   TK_IF expr TK_THEN expr TK_ELSE expr
|   TK_WHILE expr TK_DO expr
|   TK_FOR id TK_ASSIGN expr TK_TO expr TK_DO expr
|   TK_BREAK
|   TK_LET decls TK_IN expr TK_END

decls:
    /* empty */
|   decls decl

decl:
    type_decls
|   var_decl
|   func_decls

type_decls:
    TK_TYPE id TK_EQ type
|   type_decls TK_TYPE id TK_EQ type

type:
    id
|   TK_LBRACE fields TK_RBRACE
|   TK_ARRAY TK_OF id

fields:
    /* empty */
|   id TK_COLON id field_decl_seq

var_decl:
    TK_VAR id TK_ASSIGN expr
|   TK_VAR id TK_COLON id TK_ASSIGN expr

func_decls:
    func_decl
|   func_decls func_decl

func_decl:
    TK_FUNCTION id TK_LPARAN fields TK_RPARAN TK_EQ expr
|   TK_FUNCTION id TK_LPARAN fields TK_RPARAN TK_COLON id TK_EQ expr

expr_seq:
    TK_SEMICOLON expr
|   expr_seq TK_SEMICOLON expr

param_seq:
    /* empty */
|   param_seq TK_COMMA expr

field_seq:
    /* empty */
|   field_seq TK_COMMA id TK_EQ expr

field_decl_seq:
    /* empty */
|   field_decl_seq TK_COMMA id TK_COLON id

lvalue:
    id lvalue_

lvalue_:
    /* empty */
|   TK_DOT id lvalue_
|   TK_LBRACK expr TK_RBRACK lvalue_

id:
    TK_ID

%%

void yyerror(char *msg)
{
    em_error(em_tok_pos, "%s", msg);
}

static void print_token_value(FILE *fp, int type, YYSTYPE value)
{
    switch (type)
    {
        case TK_ID:
        case TK_STRING:
            fprintf(fp, "%s", value.str);
            break;
        case TK_INT:
            fprintf(fp, "%d", value.num);
            break;
        default:
            fprintf(fp, "%d", value.pos);
            break;
    }
}
