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

#include "compositor/wayland/client.h"
#include "compositor/wayland/xdg_shell.h"
#include "compositor/wayland/xdg_surface.h"
#include "protocol/xdg-shell-header.h"
#include "util/cleaner.h"
#include "util/wayland.h"

#define XDG_SHELL_VERSION 4

static void
use_unstable_version(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t version
);

/**
 *  This function is called when a surface asks for a xdg_surface
 */
static void
get_xdg_surface(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id,
    struct wl_resource* surface_resource
);

/**
 * This function is called when a surface asks for a xdg_popup
 */
static void
get_xdg_popup(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id,
    struct wl_resource* surface_resource,
    struct wl_resource* parent_resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    int32_t x,
    int32_t y,
    uint32_t flags
);

/**
 *  If we send a ping request to an application to see if it is still responding
 *  If it is, this is called
 */
static void pong(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
);

static const struct xdg_shell_interface interface = {
    .use_unstable_version = use_unstable_version,
    .get_xdg_surface = get_xdg_surface,
    .get_xdg_popup = get_xdg_popup,
    .pong = pong
};

static void bind_shell(
    struct wl_client* client,
    void* data,
    uint32_t version,
    uint32_t id
);

static struct wl_global* global;


/*
 *
 * Implementations
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_XDG_SHELL = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_wayland_xdg_shell",

    .hash_callback = NULL,
    .deinit_callback = NULL,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = NULL,
    .attribute_table = NULL,
    .function_table = NULL,
};


static void
xdg_shell_deinit(
    void* dummy
) {
    wl_global_destroy(global);
}

int
ws_wayland_xdg_shell_init(void) {
    bool is_static = false;
    if (is_static) {
        return 0;
    }
    struct wl_display* display = ws_wayland_acquire_display();

    if (!display) {
        return -1;
    }

    global = wl_global_create(display, &xdg_shell_interface, 1,
                                NULL, bind_shell);

    ws_wayland_release_display();

    ws_cleaner_add(xdg_shell_deinit, NULL);

    is_static = true;
    return 0;
}

struct ws_wayland_xdg_shell*
ws_wayland_xdg_shell_new(
    struct wl_resource* resource
) {
    struct ws_wayland_xdg_shell* self = calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }

    int retval = ws_wayland_obj_init(&self->wl_obj, resource);
    if (retval < 0) {
        free(self);
        return NULL;
    }

    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_XDG_SHELL;

    return self;
}

struct ws_waysome_xdg_shell*
ws_waysome_xdg_shell_from_resource(
    struct wl_resource* resource
) {
    if (!resource) {
        return NULL;
    }

    // check whether the resource is indeed a surface
    if (!wl_resource_instance_of(resource, &xdg_shell_interface, &interface)) {
        return NULL;
    }

    return (struct ws_waysome_xdg_shell*) wl_resource_get_user_data(resource);
}

static void
use_unstable_version(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t version
) {
    //!< @todo
}

static void
get_xdg_surface(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id,
    struct wl_resource* surface_resource
) {
    struct ws_surface* surface = ws_surface_from_resource(surface_resource);

    struct ws_wayland_xdg_surface* xdg_surface;
    xdg_surface = ws_wayland_xdg_surface_new(client,
                                             wl_resource_get_version(resource),
                                             id, surface);

    if (!xdg_surface) {
        wl_resource_post_no_memory(resource);
    }
}

static void
get_xdg_popup(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id,
    struct wl_resource* surface_resource,
    struct wl_resource* parent_resource,
    struct wl_resource* seat_resource,
    uint32_t serial,
    int32_t x,
    int32_t y,
    uint32_t flags
) {
    //!< @todo
}

static void pong(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    //!< @todo
}

static void bind_shell(
    struct wl_client* client,
    void* data,
    uint32_t version,
    uint32_t id
) {
    version = 1; // We only implement version 1 right now
    struct wl_resource* resource = ws_wayland_client_create_resource(client,
                                            &xdg_shell_interface, version, id);

    struct ws_wayland_xdg_shell* shell = ws_wayland_xdg_shell_new(resource);

    wl_resource_set_implementation(resource, &interface,
                                    shell, NULL);
}

