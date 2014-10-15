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

#include <stdbool.h>
#include <wayland-server.h>

#include "compositor/surface.h"
#include "compositor/wayland_shell.h"
#include "compositor/wayland_shell_surface.h"
#include "util/arithmetical.h"
#include "util/wayland.h"

/**
 * Version of the wayland compositor interface we're implementing
 */
#define WAYLAND_SHELL_VERSION  (1)

/**
 * Context of the compositor
 */
static struct {
    struct wl_global* shell; //!< the actual wayland shell
} wl_shell_ctx;


/*
 *
 * Forward declarations
 *
 */

void
create_shell_surface_cb(
    struct wl_client* client, //!< client requesting the surface
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial, //!< the serial to apply to the surface
    struct wl_resource* surface //!< surface to attach to the shell surface
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
static struct wl_shell_interface interface = {
    .get_shell_surface = create_shell_surface_cb,
};

/**
 * Binding function for the shell
 *
 * Creates a resource for the shell
 */
static void
bind_shell(
    struct wl_client* client, //!< client requesting the shell
    void* data, //!< userdata (not used)
    uint32_t version, //!< interface version
    uint32_t serial //!< serial to give the shell
);


/*
 *
 * Interface implementation
 *
 */

int
ws_wayland_shell_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }

    // get the display
    struct wl_display* display = ws_wayland_acquire_display();
    if (!display) {
        // there are a number of reasons why the display could not be acquired
        return -1;
    }

    // try to set up the resource
    wl_shell_ctx.shell = wl_global_create(display, &wl_shell_interface,
                                          WAYLAND_SHELL_VERSION, &interface,
                                          bind_shell);
    if (!wl_shell_ctx.shell) {
        goto cleanup_display;
    }

    // we don't need the display anymore
    ws_wayland_release_display();


    // we're done
    is_init = true;
    return 0;

cleanup_display:
    ws_wayland_release_display();
    return -1;
}


/*
 *
 * Internal implementation
 *
 */

void
create_shell_surface_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial,
    struct wl_resource* surface
) {
    struct ws_surface* internal_surface = ws_surface_from_resource(surface);

    // we create the surface and leave the destruction to the attached resource
    (void) ws_shell_surface_new(client, internal_surface, serial);
}


static void
bind_shell(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t serial
) {
    struct wl_resource* resource;
    resource = wl_resource_create(client, &wl_shell_interface,
                                  MIN(version, WAYLAND_SHELL_VERSION),
                                  serial);
    if (!resource) {
        wl_client_post_no_memory(client);
    }

    // set the implementation of the resource
    wl_resource_set_implementation(resource, &interface, NULL, NULL);
}

