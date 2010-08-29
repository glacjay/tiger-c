#ifndef INCLUDE__ENV_H
#define INCLUDE__ENV_H

#include "symbol.h"
#include "temp.h"
#include "translate.h"
#include "types.h"

typedef struct env_entry_s *env_entry_t;
struct env_entry_s
{
    enum { ENV_VAR_ENTRY, ENV_FUNC_ENTRY } kind;
    union
    {
        struct
        {
            tr_access_t access;
            type_t type;
            bool for_;
        } var;

        struct
        {
            tr_level_t level;
            tmp_label_t label;
            list_t formals;
            type_t result;
        } func;
    } u;
};
env_entry_t env_var_entry(tr_access_t access, type_t type, bool for_);
env_entry_t env_func_entry(tr_level_t level,
                           tmp_label_t label,
                           list_t formals,
                           type_t result);

table_t env_base_tenv(void);
table_t env_base_venv(void);

#endif
