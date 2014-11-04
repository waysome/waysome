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

#ifndef __WS_WAYLAND_CLIENT_H__
#define __WS_WAYLAND_CLIENT_H__

#include <wayland-server.h>
#include <wayland-util.h>

#include "objects/object.h"

/**
 * Wayland client type
 *
 * @extends ws_wayland_obj
 */
struct ws_wayland_client {
    struct ws_object obj; //!< @protected Base class
    struct wl_client* client; //!< @protected
    struct wl_list resources;
};

/**
 * Variable which holds the type information about the ws_wayland_client type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_CLIENT;

/**
 * Get an associated ws_client object, if it doesn't exist yet it will be
 * created and inserted into the global list
 *
 * @return the associated `ws_wayland_client` object or NULL on failure
 */
struct ws_wayland_client*
ws_wayland_client_get(
    struct wl_client* c //!< wl_client resource to initialize with
);

/**
 * A deleteable resource
 */
struct ws_deletable_resource {
    struct wl_resource* resource;
    struct wl_listener destroy_listener;
    struct ws_wayland_client* client;
    struct wl_list link;
};

/**
 * Initialize the wayland_client associated data structures
 *
 * @memberof ws_wayland_client
 */
int
ws_wayland_client_init(void);

/**
 * Wraps the resource creation
 *
 * @memberof ws_wayland_client
 */
struct wl_resource*
ws_wayland_client_create_resource(
    struct wl_client* client,
    const struct wl_interface* interface,
    int version,
    uint32_t id
);

#endif // __WS_WAYLAND_CLIENT_H__

