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
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/internal_context.h"
#include "compositor/wayland/client.h"
#include "compositor/wayland/pointer.h"
#include "compositor/wayland/surface.h"

/**
 * version of the wayland compositor interface we're implementing
 */
#define WAYLAND_POINTER_VERSION  (1)

/*
 *
 * Forward declarations
 *
 */

/**
 * Destroy the user data associated with a shell surface
 *
 * This method implements the destruction of the user data associated with a
 * shell surface.
 * This invalidates the object and performs an unref on it.
 *
 * @warning only call on resources which hold surfaces as constructed by
 *          ws_surface_new()
 */
static void
set_cursor(
    struct wl_client* client, //!< The wayland client associated to this req
    struct wl_resource* resource, //!< The resource of the pointer
    uint32_t serial, //!< serial of the enter event
    struct wl_resource* surface, //!< The resource of the surface
    int32_t hotspot_x, //!< The clicking hotspot on the X axis
    int32_t hotspot_y //!< The clicking hotspot on the Y axis
);

/**
 * Destroy the user data associated with a shell surface
 *
 * This method implements the destruction of the user data associated with a
 * shell surface.
 * This invalidates the object and performs an unref on it.
 *
 * @warning only call on resources which hold surfaces as constructed by
 *          ws_surface_new()
 */
static void
release(
    struct wl_client* client, //!< The wayland client associated to this req
    struct wl_resource* resource //!< The resource of the pointer
);

/**
 * Destroy the user data associated with a  pointer
 *
 * This method implements the destruction of the user data associated with a
 *  pointer.
 * This invalidates the object and performs an unref on it.
 *
 * @warning only call on resources which hold pointers as constructed by
 *          ws_wayland_pointer_new()
 */
static void
resource_destroy(
    struct wl_resource* resource //!< resource to destroy
);
/*
 *
 * Internal constants
 *
 */

/**
 * Shell surface interface definition
 */
static struct wl_pointer_interface interface = {
    .set_cursor = set_cursor,
    .release = release
};


/**
 *
 * Implementations
 *
 */
ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_POINTER = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_wayland_pointer",

    .deinit_callback    = NULL,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
    .cmp_callback       = NULL,

    .attribute_table    = NULL,
};

struct ws_wayland_pointer*
ws_wayland_pointer_new(
    struct wl_client* client,
    uint32_t serial
) {
    struct ws_wayland_pointer* self = calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client, &wl_pointer_interface,
                                                 WAYLAND_POINTER_VERSION,
                                                 serial);

    if (!resource) {
        goto cleanup_pointer;
    }

    struct ws_wayland_pointer* user_data = getref(self);
    wl_resource_set_implementation(resource, &interface, user_data,
                                   resource_destroy);

    ws_wayland_obj_init(&self->wl_obj, resource);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_POINTER;

    return self;

cleanup_pointer:
    free(self);
    return NULL;
}


bool
ws_wayland_pointer_instance_of(
    struct wl_resource* res
) {
    return wl_resource_instance_of(res,
                    &wl_pointer_interface, &interface);
}

static void
set_cursor(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial,
    struct wl_resource* surface,
    int32_t hotspot_x,
    int32_t hotspot_y
) {
    struct ws_surface* sf = ws_surface_from_resource(surface);
    struct ws_buffer* buffer = (struct ws_buffer*) &sf->img_buf.buf;

    ws_cursor_set_image(ws_comp_ctx.cursor, buffer);
    ws_cursor_set_hotspot(ws_comp_ctx.cursor, hotspot_x, hotspot_y);
}

static void
release(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //<! @todo: Implement
}

static void
resource_destroy(
    struct wl_resource* resource
) {
    struct ws_wayland_pointer* pointer;
    pointer = (struct ws_wayland_pointer*) wl_resource_get_user_data(resource);
    // we don't need a null-check since we rely on the resource to ref a pointer

    // invalidate
    ws_object_lock_write(&pointer->wl_obj.obj);
    pointer->wl_obj.resource = NULL;
    ws_object_unlock(&pointer->wl_obj.obj);
    ws_object_unref(&pointer->wl_obj.obj);
}

