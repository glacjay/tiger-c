%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "errmsg.h"
#include "symbol.h"
#include "utils.h"

int yylex(void);
void yyerror(char *msg);
%}

%union {
    int pos;
    int num;
    string_t str;
    list_t list;
    symbol_t sym;
    ast_decl_t decl;
    ast_expr_t expr;
    ast_type_t type;
    ast_var_t var;
    ast_func_t func;
}

%{
static void print_token_value(FILE *fp, int type, YYSTYPE value);
#define YYPRINT(fp, type, value) print_token_value(fp, type, value)

#define LIST_ACTION(target, prev, elem) \
    do \
    { \
        list_t p, e = list((elem), NULL); \
        (target) = p = (prev); \
        if (p) \
        { \
            while (p->next) \
                p = p->next; \
            p->next = e; \
        } \
        else \
            (target) = e; \
    } \
    while (false)
#define LVALUE_ACTION(target, prev, elem) \
    do \
    { \
        ast_var_t p, var = (elem); \
        (target) = p = (prev); \
        if (p) \
        { \
            while (p->u.field.var) \
                p = p->u.field.var; \
            p->u.field.var = var; \
        } \
        else \
            (target) = var; \
    } \
    while (false)

static ast_expr_t _program;
%}

%debug

%token <str> TK_ID TK_STRING
%token <num> TK_INT

%token <pos>
    TK_COMMA TK_COLON TK_LPARAN TK_RPARAN TK_LBRACK TK_RBRACK
    TK_LBRACE TK_RBRACE TK_DOT
    TK_ARRAY TK_IF TK_THEN TK_ELSE TK_WHILE TK_FOR TK_TO TK_LET TK_IN
    TK_END TK_OF TK_BREAK TK_NIL
    TK_FUNCTION TK_VAR TK_TYPE

%left <pos> TK_SEMICOLON
%nonassoc <pos> TK_DO
%nonassoc <pos> TK_ASSIGN
%left <pos> TK_OR
%left <pos> TK_AND
%nonassoc <pos> TK_EQ TK_NEQ TK_LT TK_LE TK_GT TK_GE
%left <pos> TK_PLUS TK_MINUS
%left <pos> TK_TIMES TK_DIVIDE
%left <pos> TK_UMINUS

%type <decl> decl var_decl
%type <expr> program expr
%type <type> type
%type <var> lvalue lvalue_
%type <list> expr_seq arg_seq efield_seq decls funcs_decl types_decl fields
%type <list> field_seq
%type <func> func_decl
%type <sym> id

%start program

%%

program:
    expr
    { _program = $1; }

expr:
    lvalue
    { $$ = ast_var_expr($1->pos, $1); }
|   TK_NIL
    { $$ = ast_nil_expr($1); }
|   expr expr_seq
    { $$ = ast_seq_expr($1->pos, list($1, $2)); }
|   TK_LPARAN TK_RPARAN
    { $$ = ast_seq_expr($1, NULL); }
|   TK_LPARAN expr TK_RPARAN
    { $$ = $2; }
|   TK_INT
    { $$ = ast_num_expr(em_tok_pos, $1); }
|   TK_STRING
    { $$ = ast_string_expr(em_tok_pos, $1); }
|   TK_MINUS expr %prec TK_UMINUS
    { $$ = ast_op_expr($1, ast_num_expr($1, 0), AST_MINUS, $2); }
|   id TK_LPARAN TK_RPARAN
    { $$ = ast_call_expr($2, $1, NULL); }
|   id TK_LPARAN expr arg_seq TK_RPARAN
    { $$ = ast_call_expr($2, $1, list($3, $4)); }
|   expr TK_PLUS expr
    { $$ = ast_op_expr($2, $1, AST_PLUS, $3); }
|   expr TK_MINUS expr
    { $$ = ast_op_expr($2, $1, AST_MINUS, $3); }
|   expr TK_TIMES expr
    { $$ = ast_op_expr($2, $1, AST_TIMES, $3); }
|   expr TK_DIVIDE expr
    { $$ = ast_op_expr($2, $1, AST_DIVIDE, $3); }
|   expr TK_EQ expr
    { $$ = ast_op_expr($2, $1, AST_EQ, $3); }
|   expr TK_NEQ expr
    { $$ = ast_op_expr($2, $1, AST_NEQ, $3); }
|   expr TK_LT expr
    { $$ = ast_op_expr($2, $1, AST_LT, $3); }
|   expr TK_LE expr
    { $$ = ast_op_expr($2, $1, AST_LE, $3); }
|   expr TK_GT expr
    { $$ = ast_op_expr($2, $1, AST_GT, $3); }
|   expr TK_GE expr
    { $$ = ast_op_expr($2, $1, AST_GE, $3); }
|   expr TK_AND expr
    { $$ = ast_if_expr($2, $1, $3, ast_num_expr($2, 0)); }
|   expr TK_OR expr
    { $$ = ast_if_expr($2, $1, ast_num_expr($2, 1), $3); }
|   id TK_LBRACE TK_RBRACE
    { $$ = ast_record_expr($2, $1, NULL); }
|   id TK_LBRACE id TK_EQ expr efield_seq TK_RBRACE
    { $$ = ast_record_expr($2, $1, list(ast_efield($4, $3, $5), $6)); }
|   id TK_LBRACK expr TK_RBRACK TK_OF expr
    { $$ = ast_array_expr($2, $1, $3, $6); }
|   lvalue TK_ASSIGN expr
    { $$ = ast_assign_expr($2, $1, $3); }
|   TK_IF expr TK_THEN expr
    { $$ = ast_if_expr($1, $2, $4, NULL); }
|   TK_IF expr TK_THEN expr TK_ELSE expr
    { $$ = ast_if_expr($1, $2, $4, $6); }
|   TK_WHILE expr TK_DO expr
    { $$ = ast_while_expr($1, $2, $4); }
|   TK_FOR id TK_ASSIGN expr TK_TO expr TK_DO expr
    { $$ = ast_for_expr($1, $2, $4, $6, $8); }
|   TK_BREAK
    { $$ = ast_break_expr($1); }
|   TK_LET decls TK_IN expr TK_END
    { $$ = ast_let_expr($1, $2, $4); }

decls:
    /* empty */
    { $$ = NULL; }
|   decls decl
    { LIST_ACTION($$, $1, $2); }

decl:
    types_decl
    { $$ = ast_types_decl(((ast_type_t) $1->data)->pos, $1); }
|   var_decl
|   funcs_decl
    { $$ = ast_funcs_decl(((ast_func_t) $1->data)->pos, $1); }

types_decl:
    TK_TYPE id TK_EQ type
    { $$ = list(ast_nametype($2, $4), NULL); }
|   types_decl TK_TYPE id TK_EQ type
    { LIST_ACTION($$, $1, ast_nametype($3, $5)); }

type:
    id
    { $$ = ast_name_type(em_tok_pos, $1); }
|   TK_LBRACE fields TK_RBRACE
    { $$ = ast_record_type($1, $2); }
|   TK_ARRAY TK_OF id
    { $$ = ast_array_type($1, $3); }

fields:
    /* empty */
    { $$ = NULL; }
|   id TK_COLON id field_seq
    { $$ = list(ast_field($1, $3), $4); }

var_decl:
    TK_VAR id TK_ASSIGN expr
    { $$ = ast_var_decl($1, $2, NULL, $4); }
|   TK_VAR id TK_COLON id TK_ASSIGN expr
    { $$ = ast_var_decl($1, $2, $4, $6); }

funcs_decl:
    func_decl
    { $$ = list($1, NULL); }
|   funcs_decl func_decl
    { LIST_ACTION($$, $1, $2); }

func_decl:
    TK_FUNCTION id TK_LPARAN fields TK_RPARAN TK_EQ expr
    { $$ = ast_func($1, $2, $4, NULL, $7); }
|   TK_FUNCTION id TK_LPARAN fields TK_RPARAN TK_COLON id TK_EQ expr
    { $$ = ast_func($1, $2, $4, $7, $9); }

expr_seq:
    TK_SEMICOLON expr
    { $$ = list($2, NULL); }
|   expr_seq TK_SEMICOLON expr
    { LIST_ACTION($$, $1, $3); }

arg_seq:
    /* empty */
    { $$ = NULL; }
|   arg_seq TK_COMMA expr
    { LIST_ACTION($$, $1, $3); }

efield_seq:
    /* empty */
    { $$ = NULL; }
|   efield_seq TK_COMMA id TK_EQ expr
    { LIST_ACTION($$, $1, ast_efield($4, $3, $5)); }

field_seq:
    /* empty */
    { $$ = NULL; }
|   field_seq TK_COMMA id TK_COLON id
    { LIST_ACTION($$, $1, ast_field($3, $5)); }

lvalue:
    id lvalue_
    { LVALUE_ACTION($$, $2, ast_simple_var(em_tok_pos, $1)); }

lvalue_:
    /* empty */
    { $$ = NULL; }
|   TK_DOT id lvalue_
    { LVALUE_ACTION($$, $3, ast_field_var($1, NULL, $2)); }
|   TK_LBRACK expr TK_RBRACK lvalue_
    { LVALUE_ACTION($$, $4, ast_sub_var($1, NULL, $2)); }

id:
    TK_ID
    { $$ = symbol($1); }

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
    }
}

ast_expr_t parse(string_t filename)
{
    em_reset(filename);
    if (yyparse() == 0)
        return _program;
    else
        return NULL;
}
