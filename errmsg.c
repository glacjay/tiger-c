#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"
#include "utils.h"

bool em_any_errors = false;
int em_tok_pos = 0;
extern FILE *yyin;

static string_t filename = "";
static int line_num = 1;

typedef struct int_list_s
{
    int i;
    struct int_list_s *next;
} *int_list_t;
static int_list_t line_pos = NULL;

static int_list_t int_list(int i, int_list_t next)
{
    int_list_t p = checked_malloc(sizeof(*p));
    p->i = i;
    p->next = next;
    return p;
}

void em_newline(void)
{
    ++line_num;
    line_pos = int_list(em_tok_pos, line_pos);
}

void em_error(int pos, string_t msg, ...)
{
    va_list ap;
    int_list_t lines = line_pos;
    int num = line_num;

    em_any_errors = true;
    while (lines && lines->i >= pos)
    {
        lines = lines->next;
        --num;
    }

    if (filename)
        fprintf(stderr, "%s:", filename);
    if (lines)
        fprintf(stderr, "%d.%d: ", num, pos - lines->i);
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void em_reset(string_t file)
{
    em_any_errors = false;
    filename = file;
    line_num = 1;
    line_pos = int_list(0, NULL);
    yyin = fopen(file, "r");
    if (!yyin)
    {
        em_error(0, "cannot open file: %s", file);
        exit(1);
    }
}
