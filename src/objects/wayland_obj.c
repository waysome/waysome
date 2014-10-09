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
#include <wayland-server.h>

#include "objects/wayland_obj.h"
#include "objects/object.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Hashing callback for `ws_wayland_obj` type
 */
static size_t
hash_callback(
    struct ws_object* const self //!< The object to hash
);

/**
 * Compare callback for `ws_wayland_obj` type
 */
int
cmp_callback(
    struct ws_object const* o1, //!< First operant
    struct ws_object const* o2 //!< Second operant
);


/**
 * Type information for ws_wayland_obj type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_OBJ = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_wayland_obj",

    .hash_callback = hash_callback,

    .init_callback = NULL,
    .deinit_callback = NULL,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = cmp_callback,
};

/*
 *
 * Interface implementation
 *
 */

int
ws_wayland_obj_init(
    struct ws_wayland_obj* self,
    struct wl_resource* r
) {
    if (!self) {
        return -EINVAL;
    }

    ws_object_init(&self->obj);

    self->obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ;
    self->resource = r;

    return 0;
}

struct ws_wayland_obj*
ws_wayland_obj_new(
    struct wl_resource* r //!< wl_object object to initialize with
) {
    struct ws_wayland_obj* w = calloc(1, sizeof(*w));

    if (!w) {
        return NULL;
    }

    if (0 == ws_wayland_obj_init(w, r)) {
        w->obj.settings |= WS_OBJECT_HEAPALLOCED;
        return w;
    }

    free(w);
    return NULL;
}

struct wl_resource*
ws_wayland_obj_get_wl_resource(
    struct ws_wayland_obj* self
) {
    if (self) {
        return self->resource;
    }

    return NULL;
}

uint32_t
ws_wayland_obj_get_id(
    struct ws_wayland_obj* self
) {
    return self->resource->object.id;
}

/*
 *
 * Static function implementations
 *
 */

static size_t
hash_callback(
    struct ws_object* const self
) {
    if (self) {
        return SIZE_MAX / ((struct ws_wayland_obj*) self)->resource->object.id;
    }

    return 0;
}

int
cmp_callback(
    struct ws_object const* o1,
    struct ws_object const* o2
) {
    struct ws_wayland_obj* w1 = (struct ws_wayland_obj*) o1;
    struct ws_wayland_obj* w2 = (struct ws_wayland_obj*) o2;

    uint32_t id1 = ws_wayland_obj_get_id(w1);
    uint32_t id2 = ws_wayland_obj_get_id(w2);

    return ((id1 == id2) ? 0 : ((id1 > id2) ? -1 : 1));
}
