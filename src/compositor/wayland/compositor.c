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

// wayland-server.h has to be included before wayland-server-protocol.h
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/internal_context.h"
#include "compositor/monitor.h"
#include "compositor/wayland/compositor.h"
#include "compositor/wayland/region.h"
#include "compositor/wayland/surface.h"
#include "objects/wayland_obj.h"
#include "util/arithmetical.h"
#include "util/wayland.h"

/**
 * Version of the wayland compositor interface we're implementing
 */
#define WAYLAND_COMPOSITOR_VERSION  (3)

/**
 * Context of the compositor
 */
static struct {
    struct wl_global* comp; //!< the actual wayland compositor
} wl_comp_ctx;

/*
 *
 * Forward declarations
 *
 */

/**
 * Create a surface
 */
void
create_surface(
    struct wl_client* client, //!< client requesting the surface
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial //!< the serial to apply to the surface
);

/**
 * Create a region
 */
void
create_region(
    struct wl_client* client, //!< the client requesting the region
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial //!< the serial to apply to the region
);

/**
 * Binding function for the compositor
 *
 * Creates a resource for the compositor
 */
static void
bind_compositor(
    struct wl_client* client, //!< client requesting the compositor
    void* data, //!< userdata (not used)
    uint32_t version, //!< interface version
    uint32_t serial //!< serial to give the compositor
);

/**
 * Add the surface to a monitor
 */
static int add_surface_to_monitor(
    void* surface_, //!< pointer to the surface
    void const* monitor_ //!< pointer to the monitor
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
static struct wl_compositor_interface interface = {
    .create_surface = create_surface,
    .create_region  = create_region,
};

/*
 *
 * Interface implementation
 *
 */

int
ws_wayland_compositor_init(void) {
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
    wl_comp_ctx.comp = wl_global_create(display, &wl_compositor_interface,
                                        WAYLAND_COMPOSITOR_VERSION, &interface,
                                        bind_compositor);
    if (!wl_comp_ctx.comp) {
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
create_surface(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    struct ws_surface* surface = ws_surface_new(client, serial);
    if (!surface) {
        //!< @todo: throw an error by emitting a signal
        return;
    }

    // TEMPORARY SOLUTION: put surface on all of the availible monitors
    //!< @todo: integrate in new infrastructure as soon as it's up
    ws_set_select(&ws_comp_ctx.monitors, NULL, NULL,
                  add_surface_to_monitor, surface);

    // we don't need the local reference any more
    ws_object_unref(&surface->wl_obj.obj);
}

void
create_region(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    (void) ws_region_new(client, serial);
}

static void
bind_compositor(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t serial
) {
    struct wl_resource* resource;
    resource = wl_resource_create(client, &wl_compositor_interface,
                                  MIN(version, WAYLAND_COMPOSITOR_VERSION),
                                  serial);
    if (!resource) {
        wl_client_post_no_memory(client);
    }

    // set the implementation of the resource
    wl_resource_set_implementation(resource, &interface, NULL, NULL);
}

static int add_surface_to_monitor(
    void* surface_,
    void const* monitor_
) {
    struct ws_monitor* monitor = (struct ws_monitor*) monitor_;
    struct ws_surface* surface = (struct ws_surface*) surface_;

    monitor = getref(monitor);

    struct ws_set* surfaces = ws_monitor_surfaces(monitor);
    if (!surfaces) {
        goto cleanup_monitor;
    }

    int retval = ws_set_insert(surfaces, ws_object_getref(&surface->wl_obj.obj));
    if (retval < 0) {
        // if the insertion failed, we unref and carry on
        ws_object_unref(&surface->wl_obj.obj);
    }

cleanup_monitor:
    ws_object_unref((struct ws_object*) monitor);
    return 0;
}

