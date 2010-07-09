#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "errmsg.h"

bool em_any_errors = false;
int em_tok_pos = 0;

extern FILE *yyin;

static string_t _filename = "";
static int _line_num = 1;
static list_t _line_pos = NULL;

void em_newline(void)
{
    ++_line_num;
    _line_pos = int_list(em_tok_pos, _line_pos);
}

void em_error(int pos, string_t msg, ...)
{
    va_list ap;
    list_t lines = _line_pos;
    int line = _line_num;

    em_any_errors = true;
    while (lines && lines->i >= pos)
    {
        lines = lines->next;
        --line;
    }

    if (_filename)
        fprintf(stderr, "%s:", _filename);
    fprintf(stderr, "%d.%d: ", line, lines ? pos - lines->i : pos);
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void em_reset(string_t filename)
{
    em_any_errors = false;
    _filename = filename;
    _line_num = 1;
    _line_pos = int_list(0, NULL);
    yyin = fopen(_filename, "r");
    if (!yyin)
    {
        em_error(0, "cannot open file: %s", _filename);
        exit(1);
    }
}
