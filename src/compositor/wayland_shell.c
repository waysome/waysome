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

#include <stdbool.h>
#include <wayland-server.h>

#include "compositor/wayland_shell.h"


/*
 *
 * Forward declarations
 *
 */

void
create_shell_surface_cb(
    struct wl_client* client, //!< client requesting the surface
    struct wl_resource* resource, //!< the resource of the compositor
    uint32_t serial, //!< the serial to apply to the surface
    struct wl_resource* surface //!< surface to attach to the shell surface
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
static struct wl_shell_interface interface = {
    .get_shell_surface = create_shell_surface_cb,
};


/*
 *
 * Interface implementation
 *
 */

int
ws_wayland_shell_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }

    // we're done
    is_init = true;
    return 0;
}


/*
 *
 * Internal implementation
 *
 */

void
create_shell_surface_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    uint32_t serial,
    struct wl_resource* surface
) {
    //!< @todo: implement
}


