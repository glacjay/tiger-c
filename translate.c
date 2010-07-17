#include "frame.h"
#include "translate.h"

struct tr_access_s
{
    tr_level_t level;
    fr_access_t access;
};

static tr_access_t tr_access(tr_level_t level, fr_access_t access)
{
    tr_access_t p = checked_malloc(sizeof(*p));
    p->level = level;
    p->access = access;
    return p;
}

struct tr_level_s
{
    tr_level_t parent;
    frame_t frame;
    list_t formals;
    list_t locals;
};

static tr_level_t _outermost = NULL;

tr_level_t tr_outermost(void)
{
    if (!_outermost)
        _outermost = tr_level(NULL, tmp_label(), NULL);
    return _outermost;
}

tr_level_t tr_level(tr_level_t parent, tmp_label_t name, list_t formals)
{
    tr_level_t p = checked_malloc(sizeof(*p));
    list_t fr_formal, q = NULL;

    p->parent = parent;
    /* extra formal for static link */
    p->frame = frame(name, bool_list(true, formals));
    fr_formal = fr_formals(p->frame);
    for (; fr_formal; fr_formal = fr_formal->next)
    {
        tr_access_t access = tr_access(p, fr_formal->data);
        if (q)
        {
            q->next = list(access, NULL);
            q = q->next;
        }
        else
            p->formals = q = list(access, NULL);
    }
    p->locals = NULL;
    return p;
}

list_t tr_formals(tr_level_t level)
{
    return level->formals;
}

tr_access_t tr_alloc_local(tr_level_t level, bool escape)
{
    fr_access_t fr_access = fr_alloc_local(level->frame, escape);
    tr_access_t access = tr_access(level, fr_access);

    if (level->locals)
    {
        list_t p = level->locals;
        while (p->next)
            p = p->next;
        p->next = list(access, NULL);
    }
    else
        level->locals = list(access, NULL);
    return access;
}
