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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <wayland-util.h>
#include <wayland-server.h>
#include <wayland-server-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include "objects/object.h"
#include "compositor/keyboard.h"

struct xkb {
    struct xkb_context * context;
    struct xkb_state * state;

    struct
    {
        struct xkb_keymap * map;
        int fd;
        uint32_t size;
        char * area;
    } keymap;

    struct
    {
        uint32_t ctrl;
        uint32_t alt;
        uint32_t super;
        uint32_t shift;
    } indices;
};

/**
 *
 */
static int
xkb_init(
    struct ws_keyboard* self //!< keyboard
);

ws_object_type_id WS_OBJECT_TYPE_ID_KEYBOARD = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_keyboard",

    .deinit_callback    = NULL,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
};

/*
 *
 * Interface implementation
 *
 */

struct ws_keyboard*
ws_keyboard_new(void)
{
    struct ws_keyboard* k = calloc(1, sizeof(*k));

    if (!k) {
        return NULL;
    }

    ws_object_init(&k->obj);

    k->obj.id = &WS_OBJECT_TYPE_ID_KEYBOARD;
    k->obj.settings |= WS_OBJECT_HEAPALLOCED;
    k->active_surface = NULL;

    wl_array_init(&k->pressed_keys);

    if (0 != xkb_init(k)) {
        ws_object_unref(&k->obj);
        return NULL;
    }

    return k;
}

struct ws_keyboard*
ws_keyboard_get(void)
{
    //!< @todo
}

void
ws_keyboard_send_keymap(
    struct ws_keyboard* self
) {
    //!< @todo
}

void
ws_keyboard_send_enter(
    struct ws_keyboard* self
) {
    //!< @todo
}

void
ws_keyboard_send_leave(
    struct ws_keyboard* self
) {
    //!< @todo
}

void
ws_keyboard_send_key(
    struct ws_keyboard* self,
    struct timeval* time,
    uint32_t key,
    uint32_t state
) {
    //!< @todo
}

void
ws_keyboard_send_modifiers(
    struct ws_keyboard* self,
    uint32_t serial,
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group
) {
    //!< @todo
}

/*
 *
 * static function implementations
 *
 */

static int
xkb_init(
    struct ws_keyboard* self //!< keyboard
) {
    //!< @todo implement
    return -1;
}

