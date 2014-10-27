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

#include <errno.h>

#include "values/union.h"
#include "values/value_type.h"

int
ws_value_union_init_from_val(
    union ws_value_union* dest,
    struct ws_value* src
) {
    ws_value_deinit(&dest->value);

    switch(src->type) {
    default:
    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
        break;

    case WS_VALUE_TYPE_NIL:
        ws_value_nil_init(&dest->nil);
        return 0;

    case WS_VALUE_TYPE_BOOL:
        ws_value_bool_init(&dest->bool_);
        {
            bool buf = ws_value_bool_get((struct ws_value_bool*) src);
            return ws_value_bool_set(&dest->bool_, buf);
        }

    case WS_VALUE_TYPE_INT:
        ws_value_int_init(&dest->int_);
        {
            uint32_t buf = ws_value_int_get((struct ws_value_int*) src);
            return ws_value_int_set(&dest->int_, buf);
        }

    case WS_VALUE_TYPE_STRING:
        ws_value_string_init(&dest->string);
        {
            struct ws_string* buf;
            buf = ws_value_string_get((struct ws_value_string*) src);
            if (!buf) {
                return -EINVAL;
            }
            ws_value_string_set_str(&dest->string, buf);
            ws_object_unref(&buf->obj);
            return 0;
        }

    case WS_VALUE_TYPE_OBJECT_ID:
        ws_value_object_id_init(&dest->object_id);
        {
            struct ws_object* buf;
            buf = ws_value_object_id_get((struct ws_value_object_id*) src);
            if (!buf) {
                return -EINVAL;
            }
            ws_value_object_id_set(&dest->object_id, buf);
            ws_object_unref(buf);
            return 0;
        }

    case WS_VALUE_TYPE_SET:
    case WS_VALUE_TYPE_NAMED:
        //!< @todo implement
        return -ENOTSUP;
    }
    return -EINVAL;
}


