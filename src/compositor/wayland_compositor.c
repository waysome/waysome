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

// wayland-server.h has to be included before wayland-server-protocol.h
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/wayland_compositor.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Create a surface
 */
void
create_surface(
    struct wl_client* client, //!< client requesting the surface
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial //!< the serial to apply to the surface
);

/**
 * Create a region
 */
void
create_region(
    struct wl_client* client, //!< the client requesting the region
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial //!< the serial to apply to the region
);

/*
 *
 * Internal constants
 *
 */

/**
 * Surface interface definition
 *
 * This interface definition holds all the methods for the surface type.
 */
static struct wl_compositor_interface interface = {
    .create_surface = create_surface,
    .create_region  = create_region,
};

/*
 *
 * Internal implementation
 *
 */

void
create_surface(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    //!< @todo: implement
}

void
create_region(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial
) {
    //!< @todo: implement
}

