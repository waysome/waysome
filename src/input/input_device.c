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

#include <ev.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "compositor/cursor.h"
#include "input/input_device.h"
#include "input/utils.h"
#include "util/arithmetical.h"

/*
 *
 * Internal constants
 *
 */

static struct ws_logger_context log_ctx = { .prefix = "[Input/Module] " };

static size_t
hash_callback(
    struct ws_object * _self
);

static int
cmp_callback(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/**
 * The callback used by libev to watch the INPUT_PATH directory
 */
static void
watch_pointers(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_io* watcher, //!< watcher which triggered the update
    int revents //!< events
);


/**
 * Type information for ws_wayland_obj type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_INPUT_DEVICE = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_input_device",

    .hash_callback = hash_callback,
    .deinit_callback = NULL,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = cmp_callback,
    .uuid_callback = NULL,
};

/*
 *
 * Interface implementations
 *
 */

struct ws_input_device*
ws_input_device_new(
    int fd
) {
    struct ws_input_device* self = calloc(1, sizeof(*self));
    ws_object_init(&self->obj);
    self->obj.id = &WS_OBJECT_TYPE_ID_INPUT_DEVICE;
    self->obj.settings |= WS_OBJECT_HEAPALLOCED;

    libevdev_new_from_fd(fd, &self->dev);
    if (!self->dev) {
        ws_log(&log_ctx, LOG_ERR, "Could not create evdev struct");
        close(fd);
        return NULL;
    }

    self->watcher.data = self;
    ev_io_init(&self->watcher, watch_pointers, fd, EV_READ);

    struct ev_loop* default_loop = ev_default_loop(EVFLAG_AUTO);
    if (!default_loop) {
        close(fd);
        return NULL;
    }

    ev_io_start(default_loop, &self->watcher);

    return self;
}

static size_t
hash_callback(
    struct ws_object * self
) {
    return SIZE_MAX / (((struct ws_input_device*) self)->fd + 1);
}

static int
cmp_callback(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_input_device* dev1 = (struct ws_input_device*) obj1;
    struct ws_input_device* dev2 = (struct ws_input_device*) obj2;
    // It's short signum function
    return signum((int32_t)(dev1->fd > dev2->fd));
}

static void
watch_pointers(
    struct ev_loop* loop,
    ev_io* watcher,
    int revents
) {
    struct ws_input_device* self = (struct ws_input_device*) watcher->data;
    struct input_event ev;

    while (true) {
        int retval = libevdev_next_event(self->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (retval == -EAGAIN) {
            break;
        }

        if (retval < 0) {
            ws_log(&log_ctx, LOG_ERR, "libevdev errored: %s", strerror(retval));
            break;
        }

        if (retval == LIBEVDEV_READ_STATUS_SYNC) {
            //!< @todo: Do something about this?
            continue;
        }

        if (retval != LIBEVDEV_READ_STATUS_SUCCESS) {
            ws_log(&log_ctx, LOG_ERR, "libevdev unspecified error %d", retval);
            continue;
        }

        if (ev.type != EV_REL) {
            continue;
        }

        //ws_log(&log_ctx, LOG_DEBUG, "It's a mouse event!");

        int x = 0;
        int y = 0;

        if (ev.code == REL_X) {
            x = ev.value;
        }
        if (ev.code == REL_Y) {
            y = ev.value;
        }

        struct ws_cursor* pointer = ws_cursor_get();

        ws_cursor_add_position(pointer, x, y);
    }
}
