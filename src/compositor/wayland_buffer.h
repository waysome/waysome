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

#ifndef __WS_WAYLAND_BUFFER_H__
#define __WS_WAYLAND_BUFFER_H__

#include "objects/wayland_obj.h"

/**
 * Wayland buffer type
 */
struct ws_wayland_buffer {
    struct ws_wayland_obj wl_obj;
};

/**
 * Variable which holds the type information about the ws_wayland_buffer type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_BUFFER;

/**
 * Initialize a `ws_wayland_buffer` object
 *
 * @return zero on success, else negative errno.h constant
 */
int
ws_wayland_buffer_init(
    struct ws_wayland_buffer* self, //!< The object itself
    struct wl_resource* r //!< wl_buffer resource to initialize with
);

/**
 * Get a newly allocated and initialized `ws_wayland_obj` object
 *
 * @return new `ws_wayland_obj` object or NULL on failure
 */
struct ws_wayland_buffer*
ws_wayland_buffer_new(
    struct wl_resource* r //!< wl_buffer resource to initialize with
);

#endif // __WS_WAYLAND_BUFFER_H__

