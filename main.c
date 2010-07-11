#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "errmsg.h"
#include "parser-wrap.h"
#include "ppast.h"
#include "semantic.h"
#include "utils.h"

int main(int argc, char **argv)
{
    ast_expr_t result;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    /* yydebug = 1; */
    if ((result = parse(argv[1])) && !em_any_errors)
        sem_trans_prog(result);

    return 0;
}
