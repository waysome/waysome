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

#ifndef __WS_CURSOR_H__
#define __WS_CURSOR_H__

#include <sys/time.h>

#include "compositor/buffer/buffer.h"
#include "compositor/buffer/image.h"
#include "objects/object.h"

/**
 * Waysome's implementation of wl_cursor
 *
 * This struct represents a cursor
 */
struct ws_cursor {
    struct ws_object obj; //<! @protected Supertype
    struct ws_framebuffer_device* cur_fb_dev; //!< @private The associated fb
    struct ws_monitor* cur_mon; //!< @private the associated monitor
    struct ws_image_buffer* default_cursor; //!< @private Buffer for a cursor
    struct ws_frame_buffer* cursor_fb; //!< @private The fb for the cursor
    int x; //!< @private position in the current monitor
    int y; //!< @private position in the current monitor
    int x_hp; //!< @private position hotspot of the monitor
    int y_hp; //!< @private position hotspot of the monitor
    struct ws_surface* active_surface;
};

/**
 * Variable which holds type information about the wl_surface type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_CURSOR;


/**
 * Create a cursor using that specific device
 *
 * @memberof ws_cursor
 *
 * @returns the created cursor
 */
struct ws_cursor*
ws_cursor_new(
    struct ws_framebuffer_device* dev,
    struct ws_image_buffer* cur
);

/**
 * Move the cursor to a position
 *
 * @memberof ws_cursor
 */
void
ws_cursor_set_position(
    struct ws_cursor* self, //<! The object
    int x, //<! The position
    int y  //<! The position
);

/**
 * Move the cursor by the amounts
 *
 * @memberof ws_cursor
 */
void
ws_cursor_add_position(
    struct ws_cursor* self, //<! The object
    int x, //<! The position
    int y  //<! The position
);

/**
 * Move the cursor to a position
 *
 * @memberof ws_cursor
 */
void
ws_cursor_set_hotspot(
    struct ws_cursor* self, //<! The object
    int x, //<! The position
    int y  //<! The position
);

/**
 * Rerender the cursor as it is
 *
 * @memberof ws_cursor
 */
void
ws_cursor_redraw(
    struct ws_cursor* self //<! The object
);

/**
 * Set a new buffer
 *
 * @memberof ws_cursor
 */
void
ws_cursor_set_image(
    struct ws_cursor* self, //<! The object
    struct ws_buffer* img //<! The buffer
);

/**
 * Set a new buffer
 *
 * @memberof ws_cursor
 */
void
ws_cursor_unset(
    struct ws_cursor*
);

/**
 * Set the monitor
 *
 * @memberof ws_cursor
 */
void
ws_cursor_set_monitor(
    struct ws_cursor* self, //<! The object
    struct ws_monitor* monitor //<! The buffer
);

/**
 * Set the state of the mouse buttons
 *
 * @memberof ws_cursor
 */
void
ws_cursor_set_button_state(
    struct ws_cursor* self, //!< The cursor
    struct timeval* time, //!< The time when it got pressed
    short code, //!< The code of the input
    int state //!< The state of the button
);

/**
 * Get the cursor pointer
 *
 * @memberof ws_cursor
 *
 * @warning, check for NULL pointers
 */
struct ws_cursor*
ws_cursor_get();

#endif // __WS_CURSOR_H__

