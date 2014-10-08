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
#include <malloc.h>

#include "compositor/wayland_buffer.h"

ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_BUFFER = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_wayland_buffer",

    .hash_callback = NULL,
    .init_callback = NULL,
    .deinit_callback = NULL,
    .log_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = NULL,
};

int
ws_wayland_buffer_init(
    struct ws_wayland_buffer* self,
    struct wl_resource* r
) {
    if (!self) {
        return -EINVAL;
    }

    ws_wayland_obj_init(&self->wl_obj, r);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_BUFFER;

    return 0;
}

struct ws_wayland_buffer*
ws_wayland_buffer_new(
    struct wl_resource* r
) {
    struct ws_wayland_buffer* w = calloc(1, sizeof(*w));
    if (!w) {
        return NULL;
    }

    if (ws_wayland_obj_init(&w->wl_obj, r) != 0) {
        goto cleanup;
    }

    return w;

cleanup:
    free(w);
    return NULL;
}


