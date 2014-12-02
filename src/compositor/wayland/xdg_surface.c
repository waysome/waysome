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

#include "protocol/xdg-shell-header.h"
#include "compositor/wayland/xdg_surface.h"
#include "compositor/wayland/client.h"

/*
 *
 * XDG Interface Defintion
 *
 */

/**
 * Called if a surface is destroyed
 */
static void
destroy(
    struct wl_client* client,
    struct wl_resource* resource
);

/**
 * Sets the parent of the resource
 */
static void
set_parent(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* parent_resource
);

/**
 * Set the 'title' of a surface
 */
static void
set_title(
    struct wl_client* client,
    struct wl_resource* resource,
    char const* title
);

/**
 * Sets the app id
 * @note We do not use it yet
 */
static void
set_app_id(
    struct wl_client* client,
    struct wl_resource* resource,
    const char* app_id
);

/**
 * Called when the surface wants to show a menu
 */
static void
show_window_menu(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    int32_t x,
    int32_t y
);

/**
 * Called when the surface wants to be moved
 */
static void
move(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial
);

/**
 * Called when the surface wants to be resized
 */
static void
resize(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    uint32_t edges
);

/**
 * Here we acknowledge the configuration of a surface
 */
static void
ack_configure(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
);

/**
 * This is called to set the window geometry
 */
static void
set_window_geometry(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
);

/**
 * A request to be maximized
 */
static void
set_maximized(
    struct wl_client* client,
    struct wl_resource* resource
);

/**
 * A request to not be maximized
 */
static void
unset_maximized(
    struct wl_client* client,
    struct wl_resource* resource
);

/**
 * A request to be fullscreen
 */
static void
set_fullscreen(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* output_resource
);

/**
 * A request to not be fullscreen anymore
 */
static void
unset_fullscreen(
    struct wl_client* client,
    struct wl_resource* resource
);

/**
 * A request to minimize yourself
 */
static void
set_minimized(
    struct wl_client* client,
    struct wl_resource* resource
);

static const struct xdg_surface_interface interface = {
    .destroy                = destroy,
    .set_parent             = set_parent,
    .set_title              = set_title,
    .set_app_id             = set_app_id,
    .show_window_menu       = show_window_menu,
    .move                   = move,
    .resize                 = resize,
    .ack_configure          = ack_configure,
    .set_window_geometry    = set_window_geometry,
    .set_maximized          = set_maximized,
    .unset_maximized        = unset_maximized,
    .set_fullscreen         = set_fullscreen,
    .unset_fullscreen       = unset_fullscreen,
    .set_minimized          = set_minimized,
};

/*
 *
 * XDG Interface Implementation
 *
 */

static void
destroy(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo implement
}

static void
set_parent(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* parent_resource
) {
    //!< @todo implement
}

static void
set_title(
    struct wl_client* client,
    struct wl_resource* resource,
    char const* title
) {
    //!< @todo implement
}

static void
set_app_id(
    struct wl_client* client,
    struct wl_resource* resource,
    const char* app_id
) {
    //!< @todo implement
}

static void
show_window_menu(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    int32_t x,
    int32_t y
) {
    //!< @todo implement
}

static void
move(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial
) {
    //!< @todo implement
}

static void
resize(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    uint32_t edges
) {
    //!< @todo implement
}

static void
ack_configure(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    //!< @todo implement
}

static void
set_window_geometry(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
) {
    //!< @todo implement
}

static void
set_maximized(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo implement
}

static void
unset_maximized(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo implement
}

static void
set_fullscreen(
    struct wl_client* client,
    struct wl_resource* resource,
    struct wl_resource* output_resource
) {
    //!< @todo implement
}

static void
unset_fullscreen(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo implement
}

static void
set_minimized(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo implement
}

struct ws_wayland_xdg_surface*
ws_wayland_xdg_surface_new(
    struct wl_client* client,
    uint32_t version,
    uint32_t id,
    struct ws_surface* surface
) {
    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client, &xdg_surface_interface,
                                                 version, id);
    if (!resource) {
        return NULL;
    }

    struct ws_wayland_xdg_surface* self = calloc(1, sizeof(*self));
    if (!self) {
        goto cleanup_wayland;
        return NULL;
    }

    int retval;
    retval = ws_abstract_shell_surface_init(&self->shell, resource, surface,
                                            &xdg_surface_interface);
    if (retval < 0) {
        goto cleanup;
        return NULL;
    }

    wl_resource_set_implementation(resource, &interface, self, NULL);

    return self;
cleanup:
    free(self);
cleanup_wayland:
    wl_resource_destroy(resource);
    return NULL;
}

