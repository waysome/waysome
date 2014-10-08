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
#include <wayland-server-protocol.h>
#include <wayland-server.h>

#include "compositor/surface.h"
#include "util/wayland.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Destroy a surface
 *
 * See wayland server library documentation for details
 */
static void
surface_destroy_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource //!< the resource affected by the action
);

/**
 * Attach a buffer to the surface
 *
 * See wayland server library documentation for details
 */
static void
surface_attach_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    struct wl_resource* buffer, //!< buffer to attach to the surface
    int32_t x, //!< change in position to display, relative to the buffer: x
    int32_t y //!< change in position to display, relative to the buffer: y
);

/**
 * Mark an area as damaged
 *
 * See wayland server library documentation for details
 */
static void
surface_damage_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    int32_t x, //!< x-coordinate of upper left corner of damaged area
    int32_t y, //!< y-coordinate of upper left corner of damaged area
    int32_t width, //!< width of damaged area
    int32_t height //!< height of damaged area
);

/**
 * Request a one-time notification on when to update the output
 *
 * See wayland server library documentation for details
 */
static void
surface_frame_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    uint32_t callback //!< callback to call when it's time for a new frame
);

/**
 * Mark a region as being opaque
 *
 * See wayland server library documentation for details
 */
static void
surface_set_opaque_region_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    struct wl_resource* region //!< region to set
);

/**
 * Mark a region as being an input region
 *
 * See wayland server library documentation for details
 */
static void
surface_set_input_region_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    struct wl_resource* region //!< region to set
);

/**
 * Commit current state (perform a flip)
 *
 * See wayland server library documentation for details
 */
static void
surface_commit_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource //!< the resource affected by the action
);

/**
 * Set a buffer transformation
 *
 * See wayland server library documentation for details
 */
static void
surface_set_buffer_transform_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    int32_t transform //!< transformation to set
);

/**
 * Set a buffer scale
 *
 * See wayland server library documentation for details
 */
static void
surface_set_buffer_scale_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the resource affected by the action
    int32_t scale //!< scale to set
);


/**
 * Destroy the user data associated with a surface
 *
 * This method implements the destruction of the user data associated with a
 * surface.
 * This invalidates the object and performs an unref on it.
 *
 * @warning only call on resources which hold surfaces as constructed by
 *          ws_surface_new()
 */
static void
resource_destroy(
    struct wl_resource* resource
);


/*
 *
 * Internal constants
 *
 */

/**
 * Surface interface definition
 *
 * This interface definition holds all the methods for the surface type.
 */
static const struct wl_surface_interface interface = {
    .destroy                = surface_destroy_cb,
    .attach                 = surface_attach_cb,
    .damage                 = surface_damage_cb,
    .frame                  = surface_frame_cb,
    .set_opaque_region      = surface_set_opaque_region_cb,
    .set_input_region       = surface_set_input_region_cb,
    .commit                 = surface_commit_cb,
    .set_buffer_transform   = surface_set_buffer_transform_cb,
    .set_buffer_scale       = surface_set_buffer_scale_cb,
};


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

    // try to get a serial
    uint32_t serial = ws_wayland_get_next_serial();
    if (!serial) {
        goto cleanup_surface;
    }

    // try to set up the resource
    struct wl_resource* resource;
    resource = wl_resource_create(client,
                                  (const struct wl_interface*) &interface,
                                  3, serial);
    if (!resource) {
        goto cleanup_surface;
    }
    wl_resource_set_user_data(resource, ws_object_getref(&self->wl_obj.obj));
    wl_resource_set_destructor(resource, resource_destroy);

    // finish the initialisation
    ws_wayland_obj_init(&self->wl_obj, resource);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_SURFACE;

    return self;

cleanup_surface:
    free(self);
    return NULL;
}


/*
 *
 * Internal implementation
 *
 */

static void
surface_destroy_cb(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo: implement
}

static void
surface_attach_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* buffer,
    int32_t x,
    int32_t y
) {
    //!< @todo: implement
}

static void
surface_damage_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
) {
    //!< @todo: implement
}

static void
surface_frame_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t callback
) {
    //!< @todo: implement
}

static void
surface_set_opaque_region_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* region
) {
    //!< @todo: implement
}

static void
surface_set_input_region_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* region
) {
    //!< @todo: implement
}

static void
surface_commit_cb(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo: implement
}

static void
surface_set_buffer_transform_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t transform
) {
    //!< @todo: implement
}

static void
surface_set_buffer_scale_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t transform
) {
    //!< @todo: implement
}

static void
resource_destroy(
    struct wl_resource* resource
) {
    struct ws_surface* surface;
    surface = (struct ws_surface*) wl_resource_get_user_data(resource);
    // we don't need a null-check since we rely on the resource to ref a surface

    // invalidate
    ws_object_lock_write(&surface->wl_obj.obj);
    surface->wl_obj.resource = NULL;
    ws_object_unlock(&surface->wl_obj.obj);
    ws_object_unref(&surface->wl_obj.obj);
}

