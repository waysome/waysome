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
#include <stdlib.h>
#include "values/set.h"

static void
value_set_deinit(
    struct ws_value* self
) {
    struct ws_value_set* s = (struct ws_value_set*) self;

    if (!s) {
        return;
    }

    ws_object_unref(&s->set->obj);
}

int
ws_value_set_init(
    struct ws_value_set* self
) {
    self->set = ws_set_new();
    if (!self) {
        return -ENOMEM;
    }

    self->value.type = WS_VALUE_TYPE_SET;
    self->value.deinit_callback = value_set_deinit;

    return 0;
}

struct ws_value_set*
ws_value_set_new(void) {
    struct ws_value_set* s = calloc(1, sizeof(*s));
    if (!s) {
        return NULL;
    }

    if (ws_value_set_init(s) < 0) {
        free(s);
        return NULL;
    }

    return s;
}

int
ws_value_set_insert(
    struct ws_value_set* self,
    struct ws_object* obj
) {
    return ws_set_insert(self->set, obj);
}

int
ws_value_set_remove(
    struct ws_value_set* self,
    struct ws_object const* cmp
) {
    return ws_set_remove(self->set, cmp);
}

struct ws_object*
ws_value_set_get(
    struct ws_value_set const* self,
    struct ws_object const* cmp
) {
    return  ws_set_get(self->set, cmp);
}

/*
int
ws_value_set_union(
    struct ws_value_set* dest,
    struct ws_value_set const* src_a,
    struct ws_value_set const* src_b
) {
    return ws_set_union(dest->set, src_a->set, src_b->set);
}
*/

/*
int
ws_value_set_intersection(
    struct ws_value_set* dest,
    struct ws_value_set const* src_a,
    struct ws_value_set const* src_b
) {
    return ws_set_intersection(dest->set, src_a->set, src_b->set);
}
*/

/*
int
ws_value_set_xor(
    struct ws_value_set* dest,
    struct ws_value_set const* src_a,
    struct ws_value_set const* src_b
) {
    return ws_set_xor(dest->set, src_a->set, src_b->set);
}
*/

/*
bool
ws_value_set_is_subset(
    struct ws_value_set const* self,
    struct ws_value_set const* other
) {
    return ws_set_is_subset(self->set, other->set);
}
*/

