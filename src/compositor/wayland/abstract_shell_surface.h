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

#ifndef __WAYSOME_ABSTRACT_SHELL_SURFACE_H__
#define __WAYSOME_ABSTRACT_SHELL_SURFACE_H__

#include "objects/wayland_obj.h"

// forward declarations
struct ws_surface; // definition in compositor/surface.h
struct wl_interface; // definition in wayland headers


/**
 * An abstract shell surface to have an unified way to access the surface.
 */
struct ws_abstract_shell_surface {
    struct ws_wayland_obj wl_obj; //!< @protected: The associated wayland object
    struct ws_surface* surface; //!< @public: The associated surface
};

/**
 * Variable which holds type information about the wl_surface type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_ABSTRACT_SHELL_SURFACE;

/**
 * Create a new abstract shell surface
 *
 * @warning only to be used in derived types
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_abstract_shell_surface_init(
    struct ws_abstract_shell_surface* self, //!< surface to initialize
    struct wl_resource* resource, //!< resource to set
    struct ws_surface* surface, //!< surface to attach
    struct wl_interface const* role //!< role to give the surface
);


#endif // __WAYSOME_ABSTRACT_SHELL_SURFACE_H__
