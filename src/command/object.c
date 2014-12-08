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

#include "command/object.h"

#include "objects/object.h"
#include "objects/string.h"
#include "values/object_id.h"
#include "values/string.h"
#include "values/union.h"
#include "values/value_type.h"

/*
 *
 * Interface implementation
 *
 */

int
ws_builtin_cmd_hastypename(
    union ws_value_union* args
) {
    if ((args[0].value.type != WS_VALUE_TYPE_OBJECT_ID) ||
            (args[1].value.type != WS_VALUE_TYPE_STRING)) {
        return -EINVAL;
    }

    struct ws_object* o = ws_value_object_id_get(&args[0].object_id);

    struct ws_string* type_name_str     = ws_value_string_get(&args[1].string);
    const char* type_name               = ws_string_raw(type_name_str);

    bool b = ws_object_has_typename(o, type_name);

    int res = ws_value_union_reinit(args, WS_VALUE_TYPE_BOOL);
    if (res != 0) {
        return res;
    }

    ws_value_bool_set(&args[0].bool_, b);

    return 0;
}

int
ws_builtin_cmd_call(
    union ws_value_union* args
) {
    if ((args[0].value.type != WS_VALUE_TYPE_OBJECT_ID) ||
            (args[1].value.type != WS_VALUE_TYPE_STRING)) {
        return -EINVAL;
    }

    struct ws_object* obj = ws_value_object_id_get(&args[0].object_id);
    if (!obj) {
        return -EINVAL;
    }

    char* name;
    {
        struct ws_string* buf = ws_value_string_get(&args[1].string);
        if (!buf) {
            return -EINVAL;
        }

        name = ws_string_raw(buf); //name is a copy
        ws_object_unref((struct ws_object*) buf);
        if (!name) {
            return -EINVAL;
        }
    }


    return ws_object_call_cmd(obj, name, args);
}

int
ws_builtin_cmd_has_meth(
    union ws_value_union* args
) {
    if ((args[0].value.type != WS_VALUE_TYPE_OBJECT_ID) ||
            (args[1].value.type != WS_VALUE_TYPE_STRING)) {
        return -EINVAL;
    }

    struct ws_object* o = ws_value_object_id_get(&args[0].object_id);

    struct ws_string* cmdstr = ws_value_string_get(&args[1].string);
    const char* cmd = ws_string_raw(cmdstr);

    bool b = ws_object_has_cmd(o, cmd);

    int res = ws_value_union_reinit(args, WS_VALUE_TYPE_BOOL);
    if (res != 0) {
        return res;
    }

    ws_value_bool_set(&args[0].bool_, b);

    return 0;
}

int
ws_builtin_cmd_has_attr(
    union ws_value_union* args
) {
    if ((args[0].value.type != WS_VALUE_TYPE_OBJECT_ID) ||
            (args[1].value.type != WS_VALUE_TYPE_STRING)) {
        return -EINVAL;
    }

    struct ws_object* o = ws_value_object_id_get(&args[0].object_id);

    struct ws_string* attrstr = ws_value_string_get(&args[1].string);
    const char* attr = ws_string_raw(attrstr);

    bool b = ws_object_has_attr(o, attr);

    int res = ws_value_union_reinit(args, WS_VALUE_TYPE_BOOL);
    if (res != 0) {
        return res;
    }

    ws_value_bool_set(&args[0].bool_, b);

    return 0;
}

int
ws_builtin_cmd_is_instance_of(
    union ws_value_union* args
) {
    if ((args[0].value.type != WS_VALUE_TYPE_OBJECT_ID) ||
        (args[1].value.type != WS_VALUE_TYPE_OBJECT_ID)) {
        return -EINVAL;
    }

    struct ws_object* obj = ws_value_object_id_get(&args[0].object_id);

    if (!ws_value_object_id_get(&args[1].object_id)) {
        return -EINVAL;
    }

    ws_object_type_id* id = ws_value_object_id_get(&args[1].object_id)->id;

    bool b = ws_object_is_instance_of(obj, id);

    int res = ws_value_union_reinit(args, WS_VALUE_TYPE_BOOL);
    if (res != 0) {
        return res;
    }

    ws_value_bool_set(&args[0].bool_, b);

    return 0;
}
