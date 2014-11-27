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

#include <malloc.h>
#include <string.h>
#include <wayland-server.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>

#include "compositor/wayland/client.h"
#include "logger/module.h"
#include "objects/set.h"
#include "util/arithmetical.h"

/*
 *
 * Forward declarations
 *
 */

static struct ws_set clients;

static struct ws_logger_context log_ctx = { .prefix = "[Compositor/Client]" };

static size_t
hash_callback(
    struct ws_object* _self
);

static int
cmp_callback(
    struct ws_object const* _obj1,
    struct ws_object const* _obj2
);

static void
destroy_callback(
    struct wl_listener* listener,
    void* data
);

/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_CLIENT = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_wayland_client",

    .hash_callback = hash_callback,
    .deinit_callback = NULL,
    .cmp_callback = cmp_callback,
    .uuid_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};

struct ws_wayland_client*
ws_wayland_client_get(
    struct wl_client* c
) {

    struct ws_wayland_client dum;
    memset(&dum, 0, sizeof(dum));
    ws_object_init((struct ws_object*) &dum);
    dum.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_CLIENT;
    dum.client = c;

    struct ws_wayland_client* found = (struct ws_wayland_client*) ws_set_get(
            &clients,
            (struct ws_object*) &dum
    );
    ws_object_deinit((struct ws_object*) &dum);

    if (found) {
        return found;
    }

    struct ws_wayland_client* self = calloc(1, sizeof(*self));
    if (!self) {
        return NULL;
    }
    ws_object_init(&self->obj);
    self->obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_CLIENT;

    // initialize members

    self->obj.settings |= WS_OBJECT_HEAPALLOCED;
    self->client = c;
    wl_list_init(&self->resources);

    ws_set_insert(&clients, (struct ws_object*) self);

    return self;
}

static void
destroy_callback(
    struct wl_listener* listener,
    void* data
) {
    struct ws_deletable_resource* resource =
        wl_container_of(listener, resource, destroy_listener);

    wl_list_remove(&resource->link);

    free(resource);
}

struct wl_resource*
ws_wayland_client_create_resource(
    struct wl_client* client,
    const struct wl_interface* interface,
    int version,
    uint32_t id
) {

    struct ws_deletable_resource* del_res = calloc(1, sizeof(*del_res));

    if (!del_res) {
        return NULL;
    }

    del_res->resource = wl_resource_create(client, interface, version, id);

    if (!del_res->resource) {
        free(del_res);
        return NULL;
    }

    del_res->destroy_listener.notify = destroy_callback;

    struct ws_wayland_client* cl = ws_wayland_client_get(client);

    wl_list_insert(&cl->resources, &del_res->link);

    ws_log(&log_ctx, LOG_DEBUG, "Clients resource list is now %d long.",
            wl_list_length(&cl->resources));

    wl_resource_add_destroy_listener(del_res->resource,
            &del_res->destroy_listener);

    del_res->client = cl;

    return del_res->resource;
}


int
ws_wayland_client_init(void) {
    int retval = ws_set_init(&clients);
    if (retval < 0) {
        return retval;
    }
    return 0;
}


static size_t
hash_callback(
    struct ws_object* _self
) {
    return (size_t)((struct ws_wayland_client*) _self)->client;
}

static int
cmp_callback(
    struct ws_object const* _obj1,
    struct ws_object const* _obj2
) {
    struct ws_wayland_client* obj1 = (struct ws_wayland_client*) _obj1;
    struct ws_wayland_client* obj2 = (struct ws_wayland_client*) _obj2;

    return signum((ssize_t)((size_t)obj1->client - (size_t)obj2->client));
}
