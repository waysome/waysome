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

#include "compositor/wayland/buffer.h"
#include "compositor/wayland/surface.h"

/**
 * An abstract shell surface to have an unified way to access the surface.
 */
struct ws_abstract_shell_surface {
    struct ws_wayland_obj wl_obj; //!< @protected: The associated wayland object
    struct ws_surface* surface; //!< @public: The associated surface
};

#endif // __WAYSOME_ABSTRACT_SHELL_SURFACE_H__
