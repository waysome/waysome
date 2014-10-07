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
#include <pthread.h>
#include <stdint.h>

#include "values/int.h"
#include "values/value.h"
#include "util/attributes.h"

void
ws_value_int_init(
    struct ws_value_int* self
) {
    if (self) {
        ws_value_init(&self->value);

        self->value.type = WS_VALUE_TYPE_INT;
        self->value.deinit_callback = NULL;
    }
}

int
ws_value_int_set(
    struct ws_value_int* self,
    uint32_t i
) {
    if (self) {
        if (self->value.type == WS_VALUE_TYPE_INT) {
            struct ws_value_int* obj = (struct ws_value_int*) self;
            obj->i = i;
            return 0;
        }
    }
    return -EINVAL;
}

uint32_t
ws_value_int_get(
    struct ws_value_int* self
) {
    if (self) {
        if (self->value.type == WS_VALUE_TYPE_INT) {
            struct ws_value_int* obj = (struct ws_value_int*) self;

            uint32_t i = obj->i;

            return i;
        }
    }
    return 0;
}

signed int
ws_value_int_cmp(
    struct ws_value_int* self,
    struct ws_value_int* other
) {
    if (self && other) {
        if (self->value.type == WS_VALUE_TYPE_INT &&
                other->value.type == WS_VALUE_TYPE_INT) {

            signed int res;

            if (self->i == other->i) {
                res = 0;
            } else {
                res = ((self->i > other->i) ? -1 : 1);
            }

            return res;
        }
    }

    return 0;
}
