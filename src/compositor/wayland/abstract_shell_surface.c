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

#include <errno.h>
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include "compositor/wayland/abstract_shell_surface.h"
#include "compositor/wayland/surface.h"
#include "values/union.h"


/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialization callback
 */
bool
shell_surface_deinit(
    struct ws_object* obj
);

/**
 * Callback command function for setting surface width
 *
 * @memberof ws_abstract_shell_surface
 *
 * Takes two parameters:
 *  1) The object id of the surface
 *  2) The width to set
 */
static int
cmd_func_set_width(
    union ws_value_union* stack // The stack to use
);

/**
 * Callback command function for setting surface height
 *
 * @memberof ws_abstract_shell_surface
 *
 * Takes two parameters:
 *  1) The object id of the surface
 *  2) The height to set
 */
static int
cmd_func_set_height(
    union ws_value_union* stack // The stack to use
);

/**
 * Callback command function for setting surface width and height
 *
 * @memberof ws_abstract_shell_surface
 *
 * Takes two parameters:
 *  1) The object id of the surface
 *  2) The width to set
 *  2) The height to set
 */
static int
cmd_func_set_width_and_height(
    union ws_value_union* stack // The stack to use
);

/**
 * Callback command function for setting surface visibility
 *
 * @memberof ws_abstract_shell_surface
 *
 * Takes two parameters:
 *  1) The object id of the surface
 *  2) The visibility to set (true = visible)
 */
static int
cmd_func_set_visibility(
    union ws_value_union* stack // The stack to use
);

/*
 *
 * Interface implementation
 *
 */

static const struct ws_object_function FUNCTIONS[] = {
    { .name = "setwidth",           .func = cmd_func_set_width },
    { .name = "setheight",          .func = cmd_func_set_height },
    { .name = "setwidthheight",     .func = cmd_func_set_width_and_height },
    { .name = "setvisibility",      .func = cmd_func_set_visibility },
    { .name = NULL,                 .func = NULL } // Iteration stopper
};

ws_object_type_id WS_OBJECT_TYPE_ID_ABSTRACT_SHELL_SURFACE = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_abstract_shell_surface",

    .deinit_callback    = shell_surface_deinit,
    .hash_callback      = NULL,
    .cmp_callback       = NULL,

    .attribute_table    = NULL,
    .function_table     = FUNCTIONS,
};

int
ws_abstract_shell_surface_init(
    struct ws_abstract_shell_surface* self,
    struct wl_resource* resource,
    struct ws_surface* surface,
    struct wl_interface const* role
) {
    // initialize the embedded wayland object
    int retval = ws_wayland_obj_init(&self->wl_obj, resource);
    if (retval < 0) {
        return retval;
    }
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_ABSTRACT_SHELL_SURFACE;

    // initialize the remaining members
    self->surface = getref(surface);
    if (!self->surface) {
        return -1;
    }

    retval = ws_surface_set_role(self->surface, role);
    if (retval < 0) {
        ws_object_unref((struct ws_object*) self->surface);
        return retval;
    }

    self->visible = false;

    // we're done
    return 0;
}

int
ws_abstract_shell_surface_set_pos(
    struct ws_abstract_shell_surface* self,
    int32_t x,
    int32_t y
) {
    //!< @todo implement
    return 0;
}

int
ws_abstract_shell_surface_set_width(
    struct ws_abstract_shell_surface* self,
    int32_t width
) {
    static const uint32_t edges = 0; //!< @todo hardcoded?
    struct ws_surface* s = (struct ws_surface*) self->surface;
    if (!s) {
        return -EINVAL;
    }

    s->width = width;

    struct wl_resource* r = ws_wayland_obj_get_wl_resource(&s->wl_obj);
    if (!r) {
        return -EINVAL;
    }

    wl_shell_surface_send_configure(r, edges, width, s->height);
    return 0;
}

int
ws_abstract_shell_surface_set_height(
    struct ws_abstract_shell_surface* self,
    int32_t height
) {
    static const uint32_t edges = 0; //!< @todo hardcoded?
    struct ws_surface* s = (struct ws_surface*) self->surface;
    if (!s) {
        return -EINVAL;
    }

    s->height = height;

    struct wl_resource* r = ws_wayland_obj_get_wl_resource(&s->wl_obj);
    if (!r) {
        return -EINVAL;
    }

    wl_shell_surface_send_configure(r, edges, s->width, height);
    return 0;
}

int
ws_abstract_shell_surface_set_width_and_height(
    struct ws_abstract_shell_surface* self,
    int32_t width,
    int32_t height
) {
    static const uint32_t edges = 0; //!< @todo hardcoded?
    struct ws_surface* s = (struct ws_surface*) self->surface;
    if (!s) {
        return -EINVAL;
    }

    s->width = width;
    s->height = height;

    struct wl_resource* r = ws_wayland_obj_get_wl_resource(&s->wl_obj);
    if (!r) {
        return -EINVAL;
    }

    wl_shell_surface_send_configure(r, edges, width, height);
    return 0;
}

/*
 *
 * Internal implementation
 *
 */

bool
shell_surface_deinit(
    struct ws_object* obj
) {
    struct ws_abstract_shell_surface* shell_surf;
    shell_surf = (struct ws_abstract_shell_surface*) obj;

    ws_object_unref((struct ws_object*) &shell_surf->surface);
    return true;
}

static int
cmd_func_set_width(
    union ws_value_union* stack // The stack to use
) {
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_OBJECT_ID) {
        return -EINVAL;
    }

    struct ws_abstract_shell_surface* self;
    self = (struct ws_abstract_shell_surface*)
            ws_value_object_id_get(&stack[0].object_id);

    // `1` is the command string itself

    if (ws_value_get_type(&stack[2].value) != WS_VALUE_TYPE_INT) {
        ws_object_unref((struct ws_object*) self);
        return -EINVAL;
    }

    if (ws_value_get_type(&stack[3].value) != WS_VALUE_TYPE_NONE) {
        ws_object_unref((struct ws_object*) self);
        return -E2BIG;
    }

    intmax_t width = ws_value_int_get(&stack[2].int_);
    if (width > INT32_MAX) {
        ws_object_unref((struct ws_object*) self);
        return -EINVAL;
    }

    int res = ws_abstract_shell_surface_set_height(self, (int32_t) width);
    ws_object_unref(&self->wl_obj.obj);
    return res;
}

static int
cmd_func_set_height(
    union ws_value_union* stack // The stack to use
) {
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_OBJECT_ID) {
        return -EINVAL;
    }

    struct ws_abstract_shell_surface* self;
    self = (struct ws_abstract_shell_surface*)
            ws_value_object_id_get(&stack[0].object_id);

    // `1` is the command string itself

    if (ws_value_get_type(&stack[2].value) != WS_VALUE_TYPE_INT) {
        ws_object_unref((struct ws_object*) self);
        return -EINVAL;
    }

    if (ws_value_get_type(&stack[3].value) != WS_VALUE_TYPE_NONE) {
        ws_object_unref((struct ws_object*) self);
        return -E2BIG;
    }

    intmax_t height = ws_value_int_get(&stack[2].int_);
    if (height > INT32_MAX) {
        ws_object_unref((struct ws_object*) self);
        return -EINVAL;
    }

    int res = ws_abstract_shell_surface_set_height(self, (int32_t) height);
    ws_object_unref(&self->wl_obj.obj);
    return res;
}

static int
cmd_func_set_width_and_height(
    union ws_value_union* stack // The stack to use
) {
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_OBJECT_ID) {
        return -EINVAL;
    }

    struct ws_abstract_shell_surface* shs;
    shs = (struct ws_abstract_shell_surface*)
            ws_value_object_id_get(&stack[0].object_id);

    // `1` is the command string itself

    int res;
    if (ws_value_get_type(&stack[2].value) != WS_VALUE_TYPE_INT ||
            ws_value_get_type(&stack[3].value) != WS_VALUE_TYPE_INT) {
        res = -EINVAL;
        goto out;
    }

    if (ws_value_get_type(&stack[4].value) != WS_VALUE_TYPE_NONE) {
        res = -E2BIG;
        goto out;
    }

    intmax_t width = ws_value_int_get(&stack[2].int_);
    intmax_t height = ws_value_int_get(&stack[3].int_);
    if (width > INT32_MAX || height > INT32_MAX) {
        res = -EINVAL;
        goto out;
    }

    res = ws_abstract_shell_surface_set_width_and_height(shs,
                                                         (int32_t) width,
                                                         (int32_t) height);

out:
    ws_object_unref(&shs->wl_obj.obj);
    return res;
}

static int
cmd_func_set_visibility(
    union ws_value_union* stack // The stack to use
) {
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_OBJECT_ID) {
        return -EINVAL;
    }

    // `1` is the command string itself

    if (ws_value_get_type(&stack[2].value) != WS_VALUE_TYPE_BOOL) {
        return -EINVAL;
    }

    struct ws_abstract_shell_surface* self;
    self = (struct ws_abstract_shell_surface*)
            ws_value_object_id_get(&stack[0].object_id);

    self->visible = ws_value_bool_get(&stack[2].bool_);

    return 0;
}
