#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"
#include "parser.h"
#include "utils.h"

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

    parse(argv[1]);

    return 0;
}
