#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "errmsg.h"
#include "escape.h"
#include "parser-wrap.h"
#include "ppast.h"
#include "semantic.h"
#include "utils.h"

int main(int argc, char **argv)
{
    ast_expr_t prog;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    /* yydebug = 1; */
    if (!(prog = parse(argv[1])) || em_any_errors)
    {
        exit(1);
    }

    esc_find_escape(prog);
    // pp_expr(stdout, 0, prog);
    sem_trans_prog(prog);

    return 0;
}
