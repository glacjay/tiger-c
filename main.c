#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "errmsg.h"
#include "parser-wrap.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    /* yydebug = 1; */
    if (parse(argv[1]))
        printf("Parsing successful!\n");
    else
        fprintf(stderr, "Parsing failed.\n");

    return 0;
}
