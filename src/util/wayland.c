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
#include <pthread.h>
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
    pthread_mutex_t lock; //!< lock to impose on the loop
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
        pthread_mutex_lock(&wayland_ctx.lock);
        return wayland_ctx.display;
    }

    // first, initialize the display singleton itself
    wayland_ctx.display = wl_display_create();
    if (!wayland_ctx.display) {
        return NULL;
    }

    int retval;

    // create the lock
    pthread_mutexattr_t attr;
    retval = pthread_mutexattr_init(&attr);
    if (retval != 0) {
        errno = retval;
        goto cleanup_display;
    }

    // we want a recursive lock
    retval = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (retval != 0) {
        errno = retval;
        goto cleanup_mutex_attr;
    }

    retval = pthread_mutex_init(&wayland_ctx.lock, &attr);
    if (retval != 0) {
        errno = retval;
        goto cleanup_mutex_attr;
    }

    pthread_mutexattr_destroy(&attr);

    // add the cleaner
    retval = ws_cleaner_add(wayland_display_cleanup, NULL);
    if (retval < 0) {
        errno = -retval;
        goto cleanup_mutex;
    }

    // everything is initialized now
    is_init = true;
    pthread_mutex_lock(&wayland_ctx.lock);
    return wayland_ctx.display;

    // cleanup code

cleanup_mutex:
    pthread_mutex_destroy(&wayland_ctx.lock);

cleanup_mutex_attr:
    pthread_mutexattr_destroy(&attr);

cleanup_display:
    wl_display_destroy(wayland_ctx.display);
    return NULL;
}

void
ws_wayland_release_display(void)
{
    pthread_mutex_unlock(&wayland_ctx.lock);
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
    pthread_mutex_destroy(&wayland_ctx.lock);
}

