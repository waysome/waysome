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
#include <wayland-util.h>

#include "objects/object.h"
#include "objects/queue.h"

struct queue_element {
    struct wl_list link;
    struct ws_object* obj;
};

/*
 * Type information
 */
ws_object_type_id WS_OBJECT_TYPE_ID_QUEUE = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_queue",

    .deinit_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,
    .attribute_table = NULL,
    .function_table = NULL,
};

int
ws_queue_init(
    struct ws_queue* q
) {
    if (!ws_object_init(&q->obj)) {
        return -EINVAL;
    }
    q->obj.id = &WS_OBJECT_TYPE_ID_QUEUE;

    wl_list_init(&q->link);

    return 0;
}

struct ws_queue*
ws_queue_new(void)
{
    struct ws_queue* q = calloc(1, sizeof(*q));
    if (!q) {
        return NULL;
    }

    if (ws_queue_init(q) < 0) {
        free(q);
        return NULL;
    }

    q->obj.settings |= WS_OBJECT_HEAPALLOCED;

    return q;
}

struct ws_object*
ws_queue_pop(
    struct ws_queue* q
) {
    //!< @todo implement
    return NULL;
}

int
ws_queue_push(
    struct ws_queue* q,
    struct ws_object* o
) {
    //!< @todo implement
    return 0;
}

bool
ws_queue_empty(
    struct ws_queue* q
) {
    //!< @todo implement
    return false;
}

