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

#ifndef __WS_KEYBOARD_H__
#define __WS_KEYBOARD_H__

#include <stdint.h>
#include <sys/time.h>
#include <xkbcommon/xkbcommon.h>
#include <wayland-server-protocol.h>

#include "objects/object.h"

/**
 *
 */
struct ws_keyboard {
    struct ws_object obj; //!< @protected Base class.
    struct xkb* xkb;
    struct ws_surface* active_surface;
    struct wl_array pressed_keys;
};

extern ws_object_type_id WS_OBJECT_TYPE_ID_KEYBOARD;

/**
 * Creates a new keyboard object, should only be called once by the compositor
 */
struct ws_keyboard*
ws_keyboard_new(void);

/**
 * Gets the global keyboard, this is available after init of the compositor
 */
struct ws_keyboard*
ws_keyboard_get(void);

/**
 * Sends the keymap to the current active surface
 */
void
ws_keyboard_send_keymap(
    struct ws_keyboard* self //!< The keyboard
);

/**
 * Sends the enter event to the current active surface
 */
void
ws_keyboard_send_enter(
    struct ws_keyboard* self //!< The keyboard
);

void
ws_keyboard_send_leave(
    struct ws_keyboard* self //!< The keyboard
);

void
ws_keyboard_send_key(
    struct ws_keyboard* self, //!< The keyboard
    struct timeval* time,
    uint32_t key,
    uint32_t state
);

void
ws_keyboard_send_modifiers(
    struct ws_keyboard* self, //!< The keyboard
    uint32_t serial, //!< serial
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group
);

/**
 * Sets the active surface for this keyboard
 */
bool
ws_keyboard_set_active_surface(
    struct ws_keyboard* self, //!< The keyboard
    struct ws_surface* nxt_surface //!< The surface to be active
);
#endif // __WS_KEYBOARD_H__

