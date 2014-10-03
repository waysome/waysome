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
#include <ev.h>
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
    ev_io dispatcher; //!< dispatching watcher
    ev_prepare flusher; //!< flushing watcher
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

/**
 * Try to acquire the display
 *
 * This function tries to acquire the display, but does not block if another
 * thread has a lock on the display.
 * If the display is locked by another thread, this method simply returns NULL.
 * Use this function where non-blocking behavior is needed.
 *
 * @warning does not initialize the display
 *
 * @return the locked display or NULL
 */
static struct wl_display*
ws_wayland_try_acquire_display(void);

/*
 * Dispatching watcher callback
 *
 * This callback dispatches wayland events
 */
static void
wayland_dispatch(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_io* watcher, //!< watcher which triggered the update
    int revents //!< events
);

/**
 * Flushing watcher callback
 *
 * This callback flushes wayland events
 */
static void
wayland_flush(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_prepare* watcher, //!< watcher which triggered the update
    int revents //!< events
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

int
ws_wayland_listen(void)
{
    struct wl_display* disp = ws_wayland_acquire_display();
    if (!disp) {
        return -errno;
    }

    // set the environment variable
    {
        const char* name = wl_display_add_socket_auto(disp);
        if (!name) {
            goto display_fail;
        }

        int retval = setenv(WAYLAND_SOCKET_PATH_ENV, name, 1);
        if (retval < 0) {
            goto display_fail;
        }
    }

    // get us some fd to poll() on
    int fd;
    {
        // get the wayland event loop
        struct wl_event_loop* wl_loop = wl_display_get_event_loop(disp);
        if (!wl_loop) {
            goto display_fail;
        }

        // get the fd to poll() on
        fd = wl_event_loop_get_fd(wl_loop);
        if (fd < 0) {
            goto display_fail;
        }
    }

    ws_wayland_release_display();

    // now get the libev loop
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (!loop) {
        return -ENOENT;
    }

    // initialize watchers
    ev_io_init(&wayland_ctx.dispatcher, wayland_dispatch, fd, EV_READ);
    ev_prepare_init(&wayland_ctx.flusher, wayland_flush);

    // start those watchers
    ev_io_start(loop, &wayland_ctx.dispatcher);
    ev_prepare_start(loop, &wayland_ctx.flusher);

    return 0;

    // failure which forces us to release the display

display_fail:
    ws_wayland_release_display();
    return -ENOENT;
}

uint32_t
ws_wayland_get_next_serial(void) {
    struct wl_display* disp = ws_wayland_acquire_display();

    // return an invalid id when the display is not availible
    if (unlikely(!disp)) {
        return 0;
    }

    uint32_t retval = wl_display_next_serial(disp);

    ws_wayland_release_display();
    return retval;
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
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (loop) {
        ev_io_stop(loop, &wayland_ctx.dispatcher);
        ev_prepare_stop(loop, &wayland_ctx.flusher);
    }

    wl_display_destroy(wayland_ctx.display);
    pthread_mutex_destroy(&wayland_ctx.lock);
}

static struct wl_display*
ws_wayland_try_acquire_display(void)
{
    if (pthread_mutex_trylock(&wayland_ctx.lock) != 0) {
        return NULL;
    }

    return wayland_ctx.display;
}

static void
wayland_dispatch(
    struct ev_loop* loop,
    ev_io* watcher,
    int revents
) {
    // if we don't get a lock, another thread is probably dispatching already
    struct wl_display* disp = ws_wayland_try_acquire_display();
    if (!disp) {
        return;
    }

    struct wl_event_loop* wl_loop = wl_display_get_event_loop(disp);
    if (unlikely(!wl_loop)) {
        return;
    }

    // dispatch events and return immediately, even if no events are pending
    wl_event_loop_dispatch(wl_loop, 0);

    ws_wayland_release_display();
}

static void
wayland_flush(
    struct ev_loop* loop,
    ev_prepare* watcher,
    int revents
) {
    // why can't you flush all the clients by the loop? Like I know...
    struct wl_display* disp = ws_wayland_acquire_display();
    if (unlikely(!disp)) {
        return;
    }
    wl_display_flush_clients(disp);

    ws_wayland_release_display();
}

