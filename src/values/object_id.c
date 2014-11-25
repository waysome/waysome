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

#include <stddef.h>

#include "objects/object.h"
#include "values/object_id.h"

static void
value_object_id_deinit(
    struct ws_value* self
) {
    struct ws_value_object_id* tmp = (struct ws_value_object_id*) self;

    if (!tmp) {
        return;
    }

    if (tmp->obj) {
        ws_object_unref(tmp->obj);
    }
}

void
ws_value_object_id_init(
    struct ws_value_object_id* self
) {
    if (!self) {
        return;
    }

    ws_value_init(&self->val);
    self->val.type = WS_VALUE_TYPE_OBJECT_ID;
    self->val.deinit_callback = value_object_id_deinit;
}

struct ws_object*
ws_value_object_id_get(
    struct ws_value_object_id* self
) {
    if (!self || !self->obj) {
        return NULL;
    }

    return getref(self->obj);
}

void
ws_value_object_id_set(
    struct ws_value_object_id* self,
    struct ws_object* obj
) {
    if (!self || !obj) {
        return;
    }

    struct ws_object* tmp = getref(obj);
    if (!tmp) {
        return;
    }

    if (self->obj) {
        ws_object_unref(self->obj);
    }

    self->obj = tmp;
}

