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
 * Type information for ws_wayland_obj type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_OBJ = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_wayland_obj",

    .hash_callback = hash_callback,

    .init_callback = NULL,
    .deinit_callback = NULL,
    .log_callback = NULL,
    .run_callback = NULL,
};

/*
 *
 * Interface implementation
 *
 */

struct ws_wayland_obj*
ws_wayland_obj_new(
    struct wl_object* wlo //!< wl_object object to initialize with
) {
    /** @todo implement */
    return NULL;
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
