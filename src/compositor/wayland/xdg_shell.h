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

#ifndef __WS_WL_XDG_SHELL_H__
#define __WS_WL_XDG_SHELL_H__

#include "compositor/wayland/buffer.h"
#include "compositor/wayland/surface.h"
#include "objects/wayland_obj.h"


/**
 * Waysome's implementation of wl_surface
 *
 * This struct represents a surface
 */
struct ws_wayland_xdg_shell {
    struct ws_wayland_obj wl_obj; //!< @protected Base class.
};

/**
 * Variable which holds type information about the wl_surface type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_XDG_SHELL;

/**
 * Init the xdg shell global
 */
int
ws_wayland_xdg_shell_init(void);



/**
 *  Get the xdg_shell from a resource or NULL if invalid
 */
struct ws_waysome_xdg_shell*
ws_waysome_xdg_shell_from_resource(
    struct wl_resource* resource
);

#endif // __WS_WL_XDG_SHELL_H__

