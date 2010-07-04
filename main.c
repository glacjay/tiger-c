#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "errmsg.h"
#include "utils.h"

/*
 * Must be the last! Because the prologue section is not inserted to the
 * parser.h file.
 */
#include "parser.h"

extern int yydebug;
int yyparse(void);

static void parse(string_t filename)
{
    em_reset(filename);
    if (yyparse() == 0)
        printf("Parsing successful!\n");
    else
        fprintf(stderr, "Parsing failed.\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    /* yydebug = 1; */
    parse(argv[1]);

    return 0;
}
