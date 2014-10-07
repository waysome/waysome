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

#include <malloc.h>

#include "compositor/surface.h"

/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_SURFACE = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_surface",

    .deinit_callback    = NULL,
    .init_callback      = NULL,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
};

struct ws_surface*
ws_surface_new(
    struct wl_client* client
) {
    struct ws_surface* self = calloc(1, sizeof(struct ws_surface));
    if (!self) {
        return NULL;
    }

    ws_wayland_obj_init(&self->wl_obj, NULL); //!< @todo: pass wayland object
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_SURFACE;

    return self;
}


