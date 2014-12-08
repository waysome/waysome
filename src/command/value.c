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

#include "command/values.h"

#include "values/union.h"

int
ws_builtin_cmd_is_type(
    union ws_value_union* args
) {
    if (args[0].value.type == WS_VALUE_TYPE_NONE ||
        args[1].value.type != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }

    struct ws_string type_name_str = ws_value_string_get(&args[1].string);
    const char* check_type = ws_string_raw(type_name_str);
    const char* value_type = ws_value_type_get_name(&args[0].value);

    bool b = ws_streq(value_type, check_type);

    int res = ws_value_union_reninit(args, WS_VALUE_TYPE_BOOL);
    if (res != 0) {
        return res;
    }

    ws_value_bool_set(&args[0].bool_, b);

    return 0;
}
