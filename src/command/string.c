/* waysome - wayland based window manager
 *
 * Copyright in alphabetical order:
 *
 * Copyright (C) 2014-2015 Julian Ganz
 * Copyright (C) 2014-2015 Manuel Messner
 * Copyright (C) 2014-2015 Marcel Müller
 * Copyright (C) 2014-2015 Matthias Beyer
 * Copyright (C) 2014-2015 Nadja Sommerfeld
 *
 * This file is part of waysome.
 *
 * waysome is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * waysome is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with waysome. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdbool.h>

#include "command/string.h"

#include "command/util.h"
#include "objects/string.h"
#include "values/bool.h"
#include "values/string.h"
#include "values/union.h"
#include "values/value.h"
#include "values/value_type.h"

/*
 *
 * Forward declarations
 *
 */

static int
get_strings_from_union(
    union ws_value_union* args,
    struct ws_string** str1,
    struct ws_string** str2
);

/*
 *
 * Interface implementation
 *
 */

int
ws_builtin_cmd_strcat(
    union ws_value_union* args
) {
    union ws_value_union* it;
    struct ws_string* val;
    struct ws_string* res = args->string.str;

    //iterate over all arguments, checking whether they are ws_value_strings
    ITERATE_ARGS_TYPE(it, args, val, string) {
        res = ws_string_cat(res, val);
        if (!res) {
            return -ENOMEM;
        }
    }
    
    if (!AT_END(it)) {
        return -EINVAL;
    }

    ws_value_union_reinit(args, WS_VALUE_TYPE_STRING);
    ws_value_string_set_str(&args->string, res);

    return 0;
}

int
ws_builtin_cmd_substr(
    union ws_value_union* args
) {
    struct ws_string* str1;
    struct ws_string* str2;

    int res;
    res = get_strings_from_union(args, &str1, &str2);

    if (res != 0) {
        return res;
    }

    bool is_substr = ws_string_substr(str1, str2);

    ws_value_union_reinit(args, WS_VALUE_TYPE_BOOL);
    ws_value_bool_set(&args->bool_, is_substr);

    return 0;
}

int
ws_builtin_cmd_strcmp(
    union ws_value_union* args
) {
    return 0;
}

/*
 *
 * Static function implementations
 *
 */
static int
get_strings_from_union(
    union ws_value_union* args,
    struct ws_string** str1,
    struct ws_string** str2
) {
    if (ws_value_get_type(&args->value) != WS_VALUE_TYPE_STRING ||
            ws_value_get_type(&args[1].value) != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }

    if (ws_value_get_type(&args[2].value) != WS_VALUE_TYPE_NONE) {
        return -E2BIG;
    }

    *str1 = ws_value_string_get(&args->string);
    *str2 = ws_value_string_get(&args[1].string);

    return 0;
}
