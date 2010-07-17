#ifndef INCLUDE__TEMP_H
#define INCLUDE__TEMP_H

#include <stdio.h>

#include "symbol.h"
#include "utils.h"

typedef struct temp_s *temp_t;
temp_t temp(void);

typedef symbol_t tmp_label_t;
tmp_label_t tmp_label(void);
tmp_label_t tmp_named_label(string_t name);
string_t tmp_name(tmp_label_t label);

typedef struct tmp_map_s *tmp_map_t;
tmp_map_t tmp_empty(void);
tmp_map_t tmp_layer_map(tmp_map_t over, tmp_map_t under);
void tmp_enter(tmp_map_t map, temp_t tmp, string_t str);
string_t tmp_lookup(tmp_map_t map, temp_t tmp);
void tmp_dump_map(FILE *fp, tmp_map_t map);

tmp_map_t tmp_map(void);

#endif
