#include "temp.h"

struct temp_s
{
    int num;
};

string_t tmp_name(tmp_label_t label)
{
    return sym_name(label);
}

static int _labels = 0;

tmp_label_t tmp_label(void)
{
    char buf[16];
    snprintf(buf, sizeof(buf), ".L%d", _labels++);
    return tmp_named_label(string(buf));
}

tmp_label_t tmp_named_label(string_t str)
{
    return symbol(str);
}

static int _temps = 100;

temp_t temp(void)
{
    temp_t p = checked_malloc(sizeof(*p));
    p->num = _temps++;
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", p->num);
        tmp_enter(tmp_map(), p, string(buf));
    }
    return p;
}

struct tmp_map_s
{
    table_t table;
    tmp_map_t under;
};

tmp_map_t tmp_map(void)
{
    static tmp_map_t map = NULL;
    if (!map)
        map = tmp_empty();
    return map;
}

static tmp_map_t new_map(table_t tab, tmp_map_t under)
{
    tmp_map_t p = checked_malloc(sizeof(*p));
    p->table = tab;
    p->under = under;
    return p;
}

tmp_map_t tmp_empty(void)
{
    return new_map(tab_empty(), NULL);
}

tmp_map_t tmp_layer_map(tmp_map_t over, tmp_map_t under)
{
    if (over == NULL)
        return under;
    else
        return new_map(over->table, tmp_layer_map(over->under, under));
}

void tmp_enter(tmp_map_t map, temp_t tmp, string_t str)
{
    assert(map && map->table);
    tab_enter(map->table, tmp, str);
}

string_t tmp_lookup(tmp_map_t map, temp_t tmp)
{
    string_t str;

    assert(map && map->table);
    str = tab_lookup(map->table, tmp);
    if (str)
        return str;
    else if (map->under)
        return tmp_lookup(map->under, tmp);
    else
        return NULL;
}

static FILE *_fp;

static void show(temp_t tmp, string_t str)
{
    fprintf(_fp, "t%d ->%s\n", tmp->num, str);
}

void tmp_dump_map(FILE *fp, tmp_map_t map)
{
    _fp = fp;
    tab_dump(map->table, (tab_dump_func_t) show);
    if (map->under)
    {
        fprintf(fp, "-------\n");
        tmp_dump_map(fp, map->under);
    }
}
