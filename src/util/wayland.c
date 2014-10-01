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

#include <errno.h>
#include <stdlib.h>

#include "util/cleaner.h"
#include "util/condition.h"
#include "util/wayland.h"

#define WAYLAND_SOCKET_PATH_ENV "WAYLAND_DISPLAY"

/*
 *
 * Internal struct declarations
 *
 */

/**
 * Wayland utility context
 *
 * This struct holds all the data we need to operate the display, including a
 * lock and watchers.
 */
static struct {
    struct wl_display* display; //!< wayland display singleton
} wayland_ctx;

/*
 *
 * Forward declarations
 *
 */

/**
 * Cleanup the wayland singleton
 *
 * @warning call only once!
 *
 * @note cleanup callback intended for use with wl_cleaner_add()
 */
static void
wayland_display_cleanup(
    void* display
);


/*
 *
 * Interface implementation
 *
 */

struct wl_display*
ws_wayland_acquire_display(void)
{
    static bool is_init = false;
    if (likely(is_init)) {
        return wayland_ctx.display;
    }

    // first, initialize the display singleton itself
    wayland_ctx.display = wl_display_create();
    if (!wayland_ctx.display) {
        return NULL;
    }

    int retval;

    // add the cleaner
    retval = ws_cleaner_add(wayland_display_cleanup, NULL);
    if (retval < 0) {
        errno = -retval;
        goto cleanup_display;
    }

    // everything is initialized now
    is_init = true;
    return wayland_ctx.display;

    // cleanup code

cleanup_display:
    wl_display_destroy(wayland_ctx.display);
    return NULL;
}


/*
 *
 * Implementation of internals
 *
 */

static void
wayland_display_cleanup(
    void* dummy
) {
    wl_display_destroy(wayland_ctx.display);
}

