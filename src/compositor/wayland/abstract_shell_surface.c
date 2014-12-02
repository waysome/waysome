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

#include <wayland-server-protocol.h>

#include "compositor/wayland/abstract_shell_surface.h"
#include "compositor/wayland/surface.h"


/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialization callback
 */
bool
shell_surface_deinit(
    struct ws_object* obj
);


/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_ABSTRACT_SHELL_SURFACE = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_abstract_shell_surface",

    .deinit_callback    = shell_surface_deinit,
    .hash_callback      = NULL,
    .cmp_callback       = NULL,

    .attribute_table    = NULL,
    .function_table     = NULL,
};

int
ws_abstract_shell_surface_init(
    struct ws_abstract_shell_surface* self,
    struct wl_resource* resource,
    struct ws_surface* surface,
    struct wl_interface const* role
) {
    // initialize the embedded wayland object
    int retval = ws_wayland_obj_init(&self->wl_obj, resource);
    if (retval < 0) {
        return retval;
    }
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_ABSTRACT_SHELL_SURFACE;

    // initialize the remaining members
    self->surface = getref(surface);
    if (!self->surface) {
        return -1;
    }

    retval = ws_surface_set_role(self->surface, role);
    if (retval < 0) {
        ws_object_unref((struct ws_object*) self->surface);
        return retval;
    }

    // we're done
    return 0;
}

int
ws_abstract_shell_surface_set_pos(
    struct ws_abstract_shell_surface* self,
    int32_t x,
    int32_t y
) {
    //!< @todo implement
    return 0;
}

int
ws_abstract_shell_surface_set_width(
    struct ws_abstract_shell_surface* self,
    int32_t width
) {
    //!< @todo implement
    return 0;
}

int
ws_abstract_shell_surface_set_height(
    struct ws_abstract_shell_surface* self,
    int32_t height
) {
    //!< @todo implement
    return 0;
}

int
ws_abstract_shell_surface_set_width_and_height(
    struct ws_abstract_shell_surface* self,
    int32_t width,
    int32_t height
) {
    static const uint32_t edges = 0; //!< @todo hardcoded?
    struct ws_surface* s = (struct ws_surface*) self->surface;
    if (!s) {
        return -EINVAL;
    }

    s->width = width;
    s->height = height;

    struct wl_resource* r = ws_wayland_obj_get_wl_resource(&s->wl_obj);
    if (!r) {
        return -EINVAL;
    }

    wl_shell_surface_send_configure(r, edges, width, height);
    return 0;
}

/*
 *
 * Internal implementation
 *
 */

bool
shell_surface_deinit(
    struct ws_object* obj
) {
    struct ws_abstract_shell_surface* shell_surf;
    shell_surf = (struct ws_abstract_shell_surface*) obj;

    ws_object_unref((struct ws_object*) &shell_surf->surface);
    return true;
}

