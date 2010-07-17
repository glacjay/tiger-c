#ifndef INCLUDE__TRANSLATE_H
#define INCLUDE__TRANSLATE_H

#include "temp.h"
#include "utils.h"

typedef struct tr_access_s *tr_access_t;
typedef struct tr_level_s *tr_level_t;

tr_level_t tr_outermost(void);
tr_level_t tr_level(tr_level_t parent, tmp_label_t name, list_t formals);
list_t tr_formals(tr_level_t level);
tr_access_t tr_alloc_local(tr_level_t level, bool escape);

#endif
