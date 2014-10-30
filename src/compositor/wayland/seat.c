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

#include "compositor/wayland/client.h"
#include "compositor/wayland/pointer.h"
#include "compositor/wayland/seat.h"
#include "logger/module.h"
#include "util/arithmetical.h"


#define WAYLAND_SEAT_VERSION (3)

/**
 *
 *  Forward Declarations
 *
 */

static struct ws_logger_context log_ctx = { .prefix = "[Compositor/Seat]" };

/**
 * Gets a reference to the current pointer
 */
static void
get_pointer(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
);

/**
 * Gets a reference to the current pointer
 */
static void
get_keyboard(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
);

/**
 * Gets a reference to the current pointer
 */
static void
get_touch(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
);

static struct {
    struct wl_global* seat; //<! The wl_seat
} wl_seat_ctx;

static struct wl_seat_interface interface = {
    .get_pointer = get_pointer,
    .get_keyboard = get_keyboard,
    .get_touch = get_touch
};

static void
bind_seat(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t serial
);

/*
 *
 * Interface implementation
 *
 */

int
ws_wayland_seat_init(void)
{
    static bool is_init = 0;
    if (is_init) {
        return 0;
    }

    // get the display
    struct wl_display* display = ws_wayland_acquire_display();
    if (!display) {
        // there are a number of reasons why the display could not be acquired
        return -1;
    }

    wl_seat_ctx.seat = wl_global_create(display, &wl_seat_interface,
                                            WAYLAND_SEAT_VERSION, &interface,
                                            bind_seat);

    if (!wl_seat_ctx.seat) {
        goto cleanup_display;
    }

    is_init = true;
    return 0;

cleanup_display:
    ws_wayland_release_display();
    return -1;
}

/*
 *
 * static function implementations
 *
 */

static void
get_pointer(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
) {
    ws_log(&log_ctx, LOG_DEBUG, "Creationg Pointer...");
    (void) ws_wayland_pointer_new(client, id);
}

static void
get_keyboard(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
) {
    //<! @todo: Implement
}

static void
get_touch(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t id
) {
    //<! @todo: Implement
}

static void
bind_seat(
    struct wl_client *client,
    void *data,
    uint32_t version,
    uint32_t serial
) {
    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client, &wl_seat_interface,
                                  MIN(version, WAYLAND_SEAT_VERSION),
                                  serial);
    if (!resource) {
        wl_client_post_no_memory(client);
    }
    wl_resource_set_implementation(resource, &interface, NULL, NULL);

    //<! @todo: Set this on a device basis
    wl_seat_send_capabilities(resource, WL_SEAT_CAPABILITY_POINTER);
}
