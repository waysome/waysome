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

#include "compositor/wayland/client.h"
#include "compositor/wayland/shell_surface.h"

/**
 * Version of the wayland surface interface we're implementing
 */
#define WAYLAND_SHELL_SURFACE_VERSION (1)

/*
 *
 * Forward declarations
 *
 */

/**
 * Reply to a ping
 */
static void
surface_pong_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    uint32_t serial //!< serial of the ping
);

/**
 * Start a pointer driven move operation
 */
static void
surface_move_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    struct wl_resource* seat, //!< seat which triggered the operation
    uint32_t serial //!< serial of the request
);

/**
 * Start a pointer driven resize
 */
static void
surface_resize_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    struct wl_resource* seat, //!< seat which triggered the operation
    uint32_t serial, //!< serial of the request
    uint32_t edges //!< constrains for the resize
);

/**
 * Make the surface a toplevel surface
 */
static void
surface_set_toplevel_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource //!< the shell surface affected
);

/**
 * Make the surface a transient surface
 */
static void
surface_set_transient_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    struct wl_resource* parent, //!< parent of the popup
    int32_t x, //!< requested position of the window
    int32_t y, //!< requested position of the window
    uint32_t flags //!< flags
);

/**
 * Make the surface a fullscreen surface
 */
static void
surface_set_fullscreen_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    uint32_t method, //!< method
    uint32_t framerate, //!< requested framerate
    struct wl_resource* output //!< optional parameter for intended output
);

/**
 * Make the surface a popup surface
 */
static void
surface_set_popup_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    struct wl_resource* seat, //!< seat the client wants to grab
    uint32_t serial, //!< some serial
    struct wl_resource* parent, //!< parent of the popup
    int32_t x, //!< requested position of the popup
    int32_t y, //!< requested position of the popup
    uint32_t flags //!< flags
);

/**
 * Maximize the surface
 */
static void
surface_set_maximized_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    struct wl_resource* output //!< optional parameter for intended output
);

/**
 * Set the title of the surface
 */
static void
surface_set_title_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    const char* title //!< intended title of the surface
);

/**
 * Set the class of the surface
 */
static void
surface_set_class_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource, //!< the shell surface affected
    const char* class //!< class of the surface
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
resource_destroy(
    struct wl_resource* resource //!< resource to destroy
);

/*
 *
 * Internal constants
 *
 */

/**
 * Shel surface interface definition
 */
static struct wl_shell_surface_interface interface = {
    .pong           = surface_pong_cb,
    .move           = surface_move_cb,
    .resize         = surface_resize_cb,
    .set_toplevel   = surface_set_toplevel_cb,
    .set_transient  = surface_set_transient_cb,
    .set_fullscreen = surface_set_fullscreen_cb,
    .set_popup      = surface_set_popup_cb,
    .set_maximized  = surface_set_maximized_cb,
    .set_title      = surface_set_title_cb,
    .set_class      = surface_set_class_cb,
};


/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_SHELL_SURFACE = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_shell_surface",

    .deinit_callback    = NULL,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
};

struct ws_shell_surface*
ws_shell_surface_new(
    struct wl_client* client,
    struct ws_surface* surface,
    uint32_t serial
) {
    struct ws_shell_surface* self = calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    // try to set up the resource
    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client,
            &wl_shell_surface_interface, WAYLAND_SHELL_SURFACE_VERSION, serial);
    if (!resource) {
        goto cleanup_surface;
    }

    // set the implementation
    struct ws_shell_surface* user_data = getref(self);
    if (!user_data) {
        goto cleanup_resource;
    }
    wl_resource_set_implementation(resource, &interface, user_data,
                                   resource_destroy);

    // finish the initialization
    ws_wayland_obj_init(&self->wl_obj, resource);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_SHELL_SURFACE;

    // initialize the members
    self->surface = getref(surface);
    if (!self->surface) {
        goto cleanup_resource;
    }

    return self;

cleanup_resource:
    wl_resource_destroy(resource);

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
surface_pong_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    //!< @todo: implement
}

static void
surface_move_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat,
    uint32_t serial
) {
    //!< @todo: implement
}

static void
surface_resize_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat,
    uint32_t serial,
    uint32_t edges
) {
    //!< @todo: implement
}

static void
surface_set_toplevel_cb(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo: implement
}

static void
surface_set_transient_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* parent,
    int32_t x,
    int32_t y,
    uint32_t flags
) {
    //!< @todo: implement
}

static void
surface_set_fullscreen_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t method,
    uint32_t framerate,
    struct wl_resource* output
) {
    //!< @todo: implement
}

static void
surface_set_popup_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat,
    uint32_t serial,
    struct wl_resource* parent,
    int32_t x,
    int32_t y,
    uint32_t flags
) {
    //!< @todo: implement
}

static void
surface_set_maximized_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* output
) {
    //!< @todo: implement
}

static void
surface_set_title_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    const char* title
) {
    //!< @todo: implement
}

static void
surface_set_class_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    const char* class
) {
    //!< @todo: implement
}

static void
resource_destroy(
    struct wl_resource* resource
) {
    struct ws_shell_surface* surface;
    surface = (struct ws_shell_surface*) wl_resource_get_user_data(resource);
    // we don't need a null-check since we rely on the resource to ref a surface

    // invalidate
    ws_object_lock_write(&surface->wl_obj.obj);
    surface->wl_obj.resource = NULL;
    ws_object_unlock(&surface->wl_obj.obj);
    ws_object_unref(&surface->wl_obj.obj);
}

