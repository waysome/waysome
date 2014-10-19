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

#include <string.h>

#include "values/value_type.h"
#include "util/arithmetical.h"

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

