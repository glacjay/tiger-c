#ifndef INCLUDE__FRAME_H
#define INCLUDE__FRAME_H

#include "temp.h"
#include "utils.h"

typedef struct frame_s *frame_t;
typedef struct fr_access_s *fr_access_t;

frame_t frame(tmp_label_t name, list_t formals);
tmp_label_t fr_name(frame_t fr);
list_t fr_formals(frame_t fr);
fr_access_t fr_alloc_local(frame_t fr, bool escape);

#endif
