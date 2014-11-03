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

// wayland-server.h has to be included before wayland-server-protocol.h
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/internal_context.h"
#include "compositor/monitor.h"
#include "compositor/buffer/buffer.h"
#include "compositor/wayland/client.h"
#include "compositor/wayland/region.h"
#include "objects/set.h"
#include "util/wayland.h"

/**
 * Version of the wayland region interface we're implementing
 */
#define WAYLAND_REGION_VERSION (1)

/*
 *
 * Forward declarations
 *
 */

/**
 * Destroy a region
 *
 * See wayland server library documentation for details
 */
static void
region_destroy_cb(
    struct wl_client* client, //!< client requesting the action
    struct wl_resource* resource //!< the resource affected by the action
);

/**
 * Add a rectangle to a region
 */
static void
region_add_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
);

/**
 * Subtract a rectangle to a region
 */
static void
region_sub_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
);

/**
 * Destroy the resource
 */
static void
resource_destroy(
    struct wl_resource* resource
);
/*
 *
 * Internal constants
 *
 */

/**
 * Region interface definition
 *
 * This interface definition holds all the methods for the region type.
 */
static struct wl_region_interface interface = {
    .destroy    = region_destroy_cb,
    .add        = region_add_cb,
    .subtract   = region_sub_cb
};


/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_REGION = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_region",

    .deinit_callback    = NULL,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
    .cmp_callback       = NULL,

    .attribute_table    = NULL,
    .function_table = NULL,
};

struct ws_region*
ws_region_new(
    struct wl_client* client,
    uint32_t serial
) {
    struct ws_region* self = calloc(1, sizeof(struct ws_region));
    if (!self) {
        return NULL;
    }

    // try to set up the resource
    struct wl_resource* resource;
    resource = ws_wayland_client_create_resource(client, &wl_region_interface,
                                  WAYLAND_REGION_VERSION, serial);
    if (!resource) {
        goto cleanup_region;
    }

    // set the implementation
    wl_resource_set_implementation(resource, &interface,
                                   ws_object_getref(&self->wl_obj.obj),
                                   resource_destroy);

    // finish the initialisation
    ws_wayland_obj_init(&self->wl_obj, resource);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_REGION;

    // initialize the members
    ws_wayland_buffer_init((struct ws_wayland_buffer*) &self->img_buf, NULL);

    return self;

cleanup_region:
    free(self);
    return NULL;
}

static void
region_destroy_cb(
    struct wl_client* client,
    struct wl_resource* resource
) {
    //!< @todo
}

static void
region_add_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
) {
    //!< @todo
}

static void
region_sub_cb(
    struct wl_client* client,
    struct wl_resource* resource,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height
) {
    //!< @todo
}

static void
resource_destroy(
    struct wl_resource* resource
) {
    struct ws_region* region;
    region = (struct ws_region*) wl_resource_get_user_data(resource);
    // we don't need a null-check since we rely on the resource to ref a region

    // invalidate
    ws_object_lock_write(&region->wl_obj.obj);
    region->wl_obj.resource = NULL;
    ws_object_unlock(&region->wl_obj.obj);
    ws_object_unref(&region->wl_obj.obj);
}

bool
ws_region_inside(
    struct ws_region* region,
    int x,
    int y
) {
    if (!region) {
        return true;
    }
    if (!ws_buffer_data((struct ws_buffer*) &region->img_buf)) {
        return true;
    }
    //!< @todo Implement really bound checking
    return false;
}

struct ws_region*
ws_region_from_resource(
    struct wl_resource* resource
) {
    if (!resource) {
        return NULL;
    }

    // check whether the resource is indeed a region
    if (!wl_resource_instance_of(resource, &wl_region_interface, &interface)) {
        return NULL;
    }

    return (struct ws_region*) wl_resource_get_user_data(resource);
}
