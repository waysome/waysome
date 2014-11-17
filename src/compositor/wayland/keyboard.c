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

#include <stdlib.h>
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/wayland/client.h"
#include "compositor/wayland/keyboard.h"
#include "objects/object.h"

#define WAYLAND_KEYBOARD_VERSION (4)

/*
 *
 * static function definitions
 *
 */

static void
release_keyboard(
    struct wl_client* client, //!< The wayland client associated to this req
    struct wl_resource* resource //!< The resource of the pointer
);

static void
resource_destroy(
    struct wl_resource* resource //!< resource to destroy
);

/*
 *
 * variables
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_KEYBOARD = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_wayland_keyboard",

    .deinit_callback    = NULL,
    .hash_callback      = NULL,
};


/**
 *
 */
static struct wl_keyboard_interface interface = {
    .release = release_keyboard,
};

/*
 *
 * interface implementations
 *
 */

struct ws_wayland_keyboard*
ws_wayland_keyboard_new(
    struct wl_client* client,
    uint32_t version,
    uint32_t serial
) {
    struct ws_wayland_keyboard* k = calloc(1, sizeof(*k));
    if (!k) {
        return NULL;
    }

    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client, &wl_keyboard_interface,
                                                 WAYLAND_KEYBOARD_VERSION,
                                                 serial);

    if (!resource) {
        goto cleanup;
    }

    struct ws_wayland_keyboard* user_data = getref(k);
    wl_resource_set_implementation(resource, &interface, user_data,
                                   resource_destroy);

    ws_wayland_obj_init(&k->wl_obj, resource);
    k->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_KEYBOARD;

    if (version >= 4) {
        wl_keyboard_send_repeat_info(resource, 500, 20);
    }

    return k;

cleanup:
    free(k);
    return NULL;
}

bool
ws_wayland_keyboard_instance_of(
    struct wl_resource* res
) {
    return wl_resource_instance_of(res, &wl_keyboard_interface, &interface);
}

/*
 *
 * static function Implementations
 *
 */

static void
release_keyboard(
    struct wl_client* client, //!< The wayland client associated to this req
    struct wl_resource* resource //!< The resource of the keyboard
) {
    //!< @todo
    return;
}

static void
resource_destroy(
    struct wl_resource* resource //!< resource to destroy
) {
    struct ws_wayland_keyboard* k;
    k = (struct ws_wayland_keyboard*) wl_resource_get_user_data(resource);
    // we don't need a null-check since we rely on the resource to ref a
    // keyboard

    // invalidate
    ws_object_lock_write(&k->wl_obj.obj);
    k->wl_obj.resource = NULL;
    ws_object_unlock(&k->wl_obj.obj);
    ws_object_unref(&k->wl_obj.obj);
}
