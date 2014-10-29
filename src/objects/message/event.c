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

#include "objects/message/event.h"
#include "objects/string.h"
#include "util/condition.h"
#include "values/union.h"

/*
 *
 * static function declarations
 *
 */

static bool
deinit_event(
    struct ws_object* self
);

/*
 *
 * Type information
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_EVENT = {
    .supertype  = &WS_OBJECT_TYPE_ID_MESSAGE,
    .typestr    = "ws_event",

    .hash_callback = NULL,
    .deinit_callback = deinit_event,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,
};

/*
 *
 * Interface implementation
 *
 */

int
ws_event_init(
    struct ws_event* self,
    struct ws_string* name,
    struct ws_value* ctx
) {
    int ret = ws_message_init(&self->m, 0);
    if (unlikely(ret != 0)) {
        goto out;
    }

    ret = ws_string_init(&self->name);
    if (unlikely(ret != 0)) {
        goto deinit_obj;
    }

    if (!ws_string_set_from_str(&self->name, name)) {
        goto deinit_obj;
    }

    ret = ws_value_union_init_from_val(&self->context, ctx);
    if (unlikely(ret != 0)) {
        goto deinit_str;
    }

    return 0;

deinit_str:
    ws_object_deinit(&self->name.obj);

deinit_obj:
    ws_object_deinit(&self->m.obj);

out:
    return ret;
}

struct ws_string*
ws_event_get_name(
    struct ws_event* self
) {
    //!< @todo implement
    return NULL;
}

union ws_value_union*
ws_event_get_context(
    struct ws_event* self
) {
    //!< @todo implement
    return NULL;
}

/*
 *
 * static function implementations
 *
 */

static bool
deinit_event(
    struct ws_object* self
) {
    //!< @todo implement
    return false;
}

