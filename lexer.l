%{
#include <ctype.h>
#include <string.h>

#include "ast.h"
#include "errmsg.h"
#include "symbol.h"
#include "parser.h"
#include "utils.h"

static int _char_pos = 1;

int yywrap(void);

#define ADJ \
do \
{ \
    em_tok_pos = yylval.pos = _char_pos; \
    _char_pos += yyleng; \
} \
while (0)

static int _comment_level = 0;

#define INIT_LEN 32
static string_t _str_buf;
static int _str_len;
static int _str_cap;
static void init_buf(void);
static void append_char(char ch);

%}

%option nounput

%x COMMENT STRING

%%

[ \t\f\v\r]             { ADJ; }
\n                      { ADJ; em_newline(); }

"/*"                    { ADJ; _comment_level = 1; BEGIN(COMMENT); }
<COMMENT>[^*/\n]*       { ADJ; }
<COMMENT>"*"+[^*/\n]*   { ADJ; }
<COMMENT>"/"+[^*/\n]*   { ADJ; }
<COMMENT>\n             { ADJ; em_newline(); }
<COMMENT>"/"+"*"        { ADJ; _comment_level++; }
<COMMENT>"*"+"/"        {
    ADJ;
    _comment_level--;
    if (_comment_level <= 0)
        BEGIN(INITIAL);
}

","                     { ADJ; return TK_COMMA; }
":"                     { ADJ; return TK_COLON; }
";"                     { ADJ; return TK_SEMICOLON; }
"("                     { ADJ; return TK_LPARAN; }
")"                     { ADJ; return TK_RPARAN; }
"["                     { ADJ; return TK_LBRACK; }
"]"                     { ADJ; return TK_RBRACK; }
"{"                     { ADJ; return TK_LBRACE; }
"}"                     { ADJ; return TK_RBRACE; }
"."                     { ADJ; return TK_DOT; }
"+"                     { ADJ; return TK_PLUS; }
"-"                     { ADJ; return TK_MINUS; }
"*"                     { ADJ; return TK_TIMES; }
"/"                     { ADJ; return TK_DIVIDE; }
"="                     { ADJ; return TK_EQ; }
"<>"                    { ADJ; return TK_NEQ; }
"<"                     { ADJ; return TK_LT; }
"<="                    { ADJ; return TK_LE; }
">"                     { ADJ; return TK_GT; }
">="                    { ADJ; return TK_GE; }
"&"                     { ADJ; return TK_AND; }
"|"                     { ADJ; return TK_OR; }
":="                    { ADJ; return TK_ASSIGN; }

array                   { ADJ; return TK_ARRAY; }
if                      { ADJ; return TK_IF; }
then                    { ADJ; return TK_THEN; }
else                    { ADJ; return TK_ELSE; }
while                   { ADJ; return TK_WHILE; }
for                     { ADJ; return TK_FOR; }
to                      { ADJ; return TK_TO; }
do                      { ADJ; return TK_DO; }
let                     { ADJ; return TK_LET; }
in                      { ADJ; return TK_IN; }
end                     { ADJ; return TK_END; }
of                      { ADJ; return TK_OF; }
break                   { ADJ; return TK_BREAK; }
nil                     { ADJ; return TK_NIL; }
function                { ADJ; return TK_FUNCTION; }
var                     { ADJ; return TK_VAR; }
type                    { ADJ; return TK_TYPE; }

[0-9]+                  { ADJ; yylval.num = atoi(yytext); return TK_INT; }
[_a-zA-Z][_a-zA-Z0-9]*  { ADJ; yylval.str = string(yytext); return TK_ID; }

\"                      { ADJ; init_buf(); BEGIN(STRING); }
<STRING>\"              {
    ADJ;
    BEGIN(INITIAL);
    yylval.str = _str_buf;
    return TK_STRING;
}
<STRING>\n              { ADJ; em_newline(); }
<STRING>\\[0-9]{3}      {
    int result;

    ADJ;
    sscanf(yytext + 1, "%d", &result);
    if (result > 0xFF)
        em_error(em_tok_pos, "character out of range");
    append_char(result);
}
<STRING>\\n             { ADJ; append_char('\n'); }
<STRING>\\t             { ADJ; append_char('\t'); }
<STRING>\\\"            { ADJ; append_char('\"'); }
<STRING>\\\\            { ADJ; append_char('\\'); }
<STRING>\\(.|\n)        { ADJ; append_char(yytext[1]); }
<STRING>[^\\\n\"]+      {
    char *p = yytext;

    ADJ;
    while (*p)
        append_char(*p++);
}

.                       { ADJ; em_error(em_tok_pos, "illegal token"); }

%%

int yywrap(void)
{
    _char_pos = 1;
    return 1;
}

static void init_buf(void)
{
    _str_buf = checked_malloc(INIT_LEN);
    _str_buf[0] = 0;
    _str_len = 0;
    _str_cap = INIT_LEN;
}

static void append_char(char ch)
{
    if (++_str_len == _str_cap)
    {
        char *p;

        _str_cap *= 2;
        p = checked_malloc(_str_cap);
        memcpy(p, _str_buf, _str_len);
        free(_str_buf);
        _str_buf = p;
    }
    _str_buf[_str_len - 1] = ch;
    _str_buf[_str_len] = 0;
}
