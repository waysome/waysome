/*
 * waysome - wayland based window manager
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

#include <stdlib.h>
#include <string.h>

#include "util/arithmetical.h"
#include "values/bool.h"
#include "values/int.h"
#include "values/nil.h"
#include "values/object_id.h"
#include "values/set.h"
#include "values/string.h"
#include "values/value_named.h"
#include "values/value_type.h"

const char* WS_VALUE_TYPE_NAMES[] = {
    [WS_VALUE_TYPE_NONE]        = "none",
    [WS_VALUE_TYPE_VALUE]       = "value",
    [WS_VALUE_TYPE_NIL]         = "nil",
    [WS_VALUE_TYPE_BOOL]        = "bool",
    [WS_VALUE_TYPE_INT]         = "int",
    [WS_VALUE_TYPE_STRING]      = "string",
    [WS_VALUE_TYPE_OBJECT_ID]   = "object",
    [WS_VALUE_TYPE_SET]         = "set",
    [WS_VALUE_TYPE_NAMED]       = "name",
};


enum ws_value_type
ws_value_type_from_value_name(
    char const* name
) {
    for (enum ws_value_type i = 0; i < ARYLEN(WS_VALUE_TYPE_NAMES); i++) {
        if (0 == strcmp(WS_VALUE_TYPE_NAMES[i], name)) {
            return i;
        }
    }

    return WS_VALUE_TYPE_NONE;
}

struct ws_value*
ws_value_type_instance_from_value_name(
    char const* name
) {
    struct ws_value* v = NULL;

    switch (ws_value_type_from_value_name(name)) {
    case WS_VALUE_TYPE_NIL:
        v = calloc(1, sizeof(struct ws_value_nil));
        ws_value_nil_init((struct ws_value_nil*) v);
        break;

    case WS_VALUE_TYPE_BOOL:
        v = calloc(1, sizeof(struct ws_value_bool));
        ws_value_bool_init((struct ws_value_bool*) v);
        break;

    case WS_VALUE_TYPE_INT:
        v = calloc(1, sizeof(struct ws_value_int));
        ws_value_int_init((struct ws_value_int*) v);
        break;

    case WS_VALUE_TYPE_STRING:
        v = calloc(1, sizeof(struct ws_value_string));
        ws_value_string_init((struct ws_value_string*) v);
        break;

//    case WS_VALUE_TYPE_OBJECT_ID:
//        v = calloc(1, sizeof(struct ws_value_obj_id));
//        ws_value_obj_id_init((struct ws_value_obj_id*) v);
//        break;
//!< @todo implement object id value type and enable this (adapt names)

    case WS_VALUE_TYPE_SET:
        v = calloc(1, sizeof(struct ws_value_set));
        ws_value_set_init((struct ws_value_set*) v);
        break;

    case WS_VALUE_TYPE_NAMED:
        v = calloc(1, sizeof(struct ws_value_named_value));
        ws_value_named_value_init((struct ws_value_named_value*) v);
        break;

    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
    default:
        v = NULL;
        break;
    }

    return v;
}

const char*
ws_value_type_get_name(
    struct ws_value* self
) {
    return WS_VALUE_TYPE_NAMES[ws_value_get_type(self)];
}

