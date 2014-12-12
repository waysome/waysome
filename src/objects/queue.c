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

static bool
deinit_queue(
    struct ws_object* const q
);

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

    .deinit_callback = deinit_queue,
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
    if (!q || wl_list_empty(&q->link)) {
        return NULL;
    }

    if (!ws_object_lock_write(&q->obj)) {
        return NULL;
    }

    struct queue_element* qe = wl_container_of(q->link.prev, qe, link);
    struct ws_object* res = qe->obj;

    wl_list_remove(&qe->link);
    free(qe);

    ws_object_unlock(&q->obj);
    return res;
}

int
ws_queue_push(
    struct ws_queue* q,
    struct ws_object* o
) {
    if (!q || !o) {
        return -EINVAL;
    }

    if (!ws_object_lock_write(&q->obj)) {
        return -EAGAIN;
    }

    struct queue_element* qe = calloc(1, sizeof(*qe));
    if (!qe) {
        ws_object_unlock(&q->obj);
        return -ENOMEM;
    }

    qe->obj = getref(o);
    if (!qe->obj) {
        free(qe);
        return -EINVAL;
    }

    wl_list_insert(&q->link, &qe->link);

    ws_object_unlock(&q->obj);
    return 0;
}

bool
ws_queue_empty(
    struct ws_queue* q
) {
    //!< @todo implement
    return false;
}

/*
 *
 * static function implementations
 *
 */

static bool
deinit_queue(
    struct ws_object* const q
) {
    struct ws_queue* queue = (struct ws_queue*) q;

    struct queue_element* iter;
    struct queue_element* tmp;
    wl_list_for_each_safe(iter, tmp, &queue->link, link) {
        ws_object_unref(iter->obj);
        wl_list_remove(&iter->link);
    }

    return true;
}
