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

/**
 * @addtogroup objects "Classes"
 *
 * @{
 */

/**
 * @addtogroup objects_wayland_obj "Class: wayland_obj type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_WAYLAND_OBJ_H__
#define __WS_OBJECTS_WAYLAND_OBJ_H__

#include <stdint.h>

#include "objects/object.h"

/**
 * ws_wayland_obj type definition
 *
 * @extends ws_object
 */
struct ws_wayland_obj {
    struct ws_object obj; //!< @protected Base class.

    struct wl_resource* resource; //!< @private Wayland object
};

/**
 * Variable which holds the type information about the ws_wayland_obj type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_OBJ;

/**
 * Initialize a `ws_wayland_obj` object
 *
 * @return zero on success, else negative errno.h constant
 */
int
ws_wayland_obj_init(
    struct ws_wayland_obj* self, //!< The object itself
    struct wl_resource* r //!< wl_object object to initialize with
);

/**
 * Get a newly allocated and initialized `ws_wayland_obj` object
 *
 * @return new `ws_wayland_obj` object or NULL on failure
 */
struct ws_wayland_obj*
ws_wayland_obj_new(
    struct wl_resource* r //!< wl_object object to initialize with
);

/**
 * Get the wl_object as wl_resource which is encapsulated in the ws_wayland_obj
 * object
 *
 * @return the wl_resource instance from the object
 */
struct wl_resource*
ws_wayland_obj_get_wl_resource(
    struct ws_wayland_obj* self //!< The object
);

/**
 * Set the wl_resource to encapsulate in the ws_wayland_obj object
 */
void
ws_wayland_obj_set_wl_resource(
    struct ws_wayland_obj* self, //!< The object
    struct wl_resource* resource //!< resource to encapsulate
);

/**
 * Get the ID of the underlying wl_object object
 *
 * @warning Not NULL safe.
 *
 * @return the ID of the underlying wl_object object
 */
uint32_t
ws_wayland_obj_get_id(
    struct ws_wayland_obj* self //!< The object
);

#endif // __WS_OBJECTS_WAYLAND_OBJ_H__

/**
 * @}
 */

/**
 * @}
 */
