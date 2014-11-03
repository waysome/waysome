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
#include <wayland-server-protocol.h>

#include "compositor/cursor.h"
#include "compositor/keyboard.h"
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

    .attribute_table = NULL,
    .function_table = NULL,
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

    if (libevdev_has_event_code(self->dev, EV_KEY, KEY_A)) {
        ws_log(&log_ctx, LOG_DEBUG, "Device is a keyboard");
        self->capabilities |= WL_SEAT_CAPABILITY_KEYBOARD;
    }

    if (libevdev_has_event_code(self->dev, EV_REL, REL_Y) &&
            libevdev_has_event_code(self->dev, EV_REL, REL_X) &&
            libevdev_has_event_code(self->dev, EV_KEY, BTN_MOUSE)) {
        ws_log(&log_ctx, LOG_DEBUG, "Device is a mouse");
        self->capabilities |= WL_SEAT_CAPABILITY_POINTER;
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
handle_relative_event(
    struct input_event* ev
) {
    //ws_log(&log_ctx, LOG_DEBUG, "It's a mouse event!");

    switch (ev->code) {
    case REL_X:
        {
            struct ws_cursor* pointer = ws_cursor_get();
            ws_cursor_add_position(pointer, ev->value, 0);
            break;
        }
    case REL_Y:
        {
            struct ws_cursor* pointer = ws_cursor_get();
            ws_cursor_add_position(pointer, 0, ev->value);
            break;
        }
    }


}

static void
handle_mouse_click_event(
    struct input_event* ev
) {
    ws_log(&log_ctx, LOG_DEBUG, "It's a mouse click! %x", ev->code);

    int state = ev->value ? WL_POINTER_BUTTON_STATE_PRESSED :
                            WL_POINTER_BUTTON_STATE_RELEASED;

    struct ws_cursor* pointer = ws_cursor_get();
    ws_cursor_set_button_state(pointer, &ev->time, ev->code, state);
}

static void
handle_keyboard_press_event(
    struct input_event* ev
) {
    ws_log(&log_ctx, LOG_DEBUG, "It's a keyboard press! %x", ev->code);

    int state = ev->value ? WL_KEYBOARD_KEY_STATE_PRESSED :
                            WL_KEYBOARD_KEY_STATE_RELEASED;

    struct ws_keyboard* k = ws_keyboard_get();

    // XKB map has an offset of 8 to linux/input.h concerning to keycodes
    ws_keyboard_send_key(k, &ev->time, ev->code + 8, state);
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
        int retval = libevdev_next_event(self->dev, LIBEVDEV_READ_FLAG_NORMAL,
                                         &ev);
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

        if (ev.type == EV_REL) {
            handle_relative_event(&ev);
            continue;
        }

        if (ev.type == EV_KEY) {
            if (BTN_MISC <= ev.code && ev.code <= BTN_GEAR_UP) {
                handle_mouse_click_event(&ev);
                continue;
            }
            if (KEY_ESC <= ev.code && ev.code <= KEY_MICMUTE) {
                handle_keyboard_press_event(&ev);
                continue;
            }
        }

        ws_log(&log_ctx, LOG_DEBUG, "Unhandled event: %x %x", ev.type, ev.code);

    }
}
