#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"
#include "tokens.h"
#include "utils.h"

YYSTYPE yylval;
int yylex(void);

string_t tok_names[] = {
    "ID", "STRING", "INT", "COMMA", "COLON", "SEMICOLON", "LPARAN",
    "RPARAN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS",
    "MINUS", "TIMES", "DIVIDE", "EQ", "NEQ", "LT", "LE", "GT", "GE",
    "AND", "OR", "ASSIGN", "ARRAY", "IF", "THEN", "ELSE", "WHILE", "FOR",
    "TO", "DO", "LET", "IN", "END", "OF", "BREAK", "NIL", "FUNCTION",
    "VAR", "TYPE",
};

static string_t tok_name(int tok)
{
    return tok < 257 || tok > 299 ? "BAK_TOKEN" : tok_names[tok - 257];
}

int main(int argc, char **argv)
{
    string_t file;
    int tok;

    if (argc != 2)
    {
        fprintf(stderr, "usage: ./a.out filename\n");
        exit(1);
    }
    file = argv[1];

    em_reset(file);
    while (true)
    {
        tok = yylex();
        if (tok == 0)
            break;
        switch (tok)
        {
            case TK_ID:
            case TK_STRING:
                printf("%10s %4d %s\n", tok_name(tok), em_tok_pos, yylval.str);
                break;
            case TK_INT:
                printf("%10s %4d %d\n", tok_name(tok), em_tok_pos, yylval.num);
                break;
            default:
                printf("%10s %4d\n", tok_name(tok), em_tok_pos);
                break;
        }
    }

    return 0;
}
