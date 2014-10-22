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

#include "values/utils.h"

#include "values/bool.h"
#include "values/int.h"
#include "values/nil.h"
#include "values/object_id.h"
#include "values/set.h"
#include "values/string.h"
#include "values/value.h"
#include "values/value_named.h"
#include "values/value_type.h"

bool
ws_value_is_nil(
    struct ws_value* self
) {
    return ws_value_get_type(self) == WS_VALUE_TYPE_NIL;
}

bool
ws_value_is_true(
    struct ws_value* self
) {
    switch (ws_value_get_type(self)) {
    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
    case WS_VALUE_TYPE_NIL:
        return false;

    case WS_VALUE_TYPE_BOOL:
        return ws_value_bool_plain((struct ws_value_bool*) self);

    case WS_VALUE_TYPE_INT:
        return ws_value_int_get((struct ws_value_int*) self) != 0;

    default:
        return true;
    }
}
