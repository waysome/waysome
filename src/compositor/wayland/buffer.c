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

#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#include <errno.h>
#include <malloc.h>
#include <wayland-server.h>
#include <wayland-util.h>

#include "compositor/internal_context.h"
#include "compositor/texture.h"
#include "compositor/wayland/buffer.h"
#include "logger/module.h"
#include "util/egl.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Get the buffer's contents
 *
 * @return the buffer's contents
 */
static void*
shm_get_data(
    struct ws_buffer const* self
);

/**
 * Get the buffer's width
 *
 * @return width of the buffer's contents
 */
static int32_t
shm_get_width(
    struct ws_buffer const* self
);

/**
 * Get the buffer's height
 *
 * @return height of the buffer's contents
 */
static int32_t
shm_get_height(
    struct ws_buffer const* self
);

/**
 * Get the buffer's stride
 *
 * @return height of the buffer's contents
 */
static int32_t
shm_get_stride(
    struct ws_buffer const* self
);

/**
 * Get the buffer's format
 *
 * @return height of the buffer's contents
 */
struct ws_egl_fmt const*
shm_get_format(
    struct ws_buffer const* self
);

/**
 * Get the buffer's format
 *
 * @return height of the buffer's contents
 */
static int
shm_transfer2texture(
    struct ws_buffer const* self,
    struct ws_texture* texture
);

/**
 * Begin a transaction
 */
static void
shm_begin_access(
    struct ws_buffer* self
);

/**
 * End a transaction
 */
static void
shm_end_access(
    struct ws_buffer* self
);

/**
 * Get the (egl) buffer's format
 *
 * @return height of the buffer's contents
 */
static int
egl_transfer2texture(
    struct ws_buffer const* self,
    struct ws_texture* texture
);

/**
 * Begin a transaction
 */
static void
egl_begin_access(
    struct ws_buffer* self
);

/**
 * End a transaction
 */
static void
egl_end_access(
    struct ws_buffer* self
);

/*
 *
 * Internal constant
 *
 */

static struct ws_logger_context log_ctx = { .prefix = "[Compositor/Surface] " };

/**
 * Buffer type
 */
static ws_buffer_type_id shm_buffer_type = {
    .type = {
        .supertype  = (struct ws_object_type const*) &WS_OBJECT_TYPE_ID_BUFFER,
        .typestr    = "anonymous",

        .hash_callback = NULL,

        .deinit_callback = NULL,
        .cmp_callback = NULL,

        .attribute_table = NULL,
        .function_table = NULL,
    },
    .get_data = shm_get_data,
    .get_width = shm_get_width,
    .get_height = shm_get_height,
    .get_stride = shm_get_stride,
    .get_format = shm_get_format,
    .transfer2texture = shm_transfer2texture,
    .begin_access = shm_begin_access,
    .end_access = shm_end_access,
};

static ws_buffer_type_id egl_buffer_type = {
    .type = {
        .supertype  = (struct ws_object_type const*) &WS_OBJECT_TYPE_ID_BUFFER,
        .typestr    = "anonymous",

        .hash_callback = NULL,

        .deinit_callback = NULL,
        .cmp_callback = NULL,

        .attribute_table = NULL,
        .function_table = NULL,
    },
    .get_data           = NULL,
    .get_width          = NULL,
    .get_height         = NULL,
    .get_stride         = NULL,
    .get_format         = NULL,
    .transfer2texture   = egl_transfer2texture,
    .begin_access       = egl_begin_access,
    .end_access         = egl_end_access,
};

/*
 *
 * Interface implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_WAYLAND_BUFFER = {
    .supertype  = &WS_OBJECT_TYPE_ID_WAYLAND_OBJ,
    .typestr    = "ws_wayland_buffer",

    .hash_callback = NULL,
    .deinit_callback = NULL,
    .cmp_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};

int
ws_wayland_buffer_init(
    struct ws_wayland_buffer* self,
    struct wl_resource* r
) {
    if (!self) {
        return -EINVAL;
    }

    ws_wayland_obj_init(&self->wl_obj, r);
    self->wl_obj.obj.id = &WS_OBJECT_TYPE_ID_WAYLAND_BUFFER;

    // initialize members
    int retval = ws_buffer_init(&self->buf);
    if (retval < 0) {
        return retval;
    }

    ws_wayland_buffer_set_resource(self, r); // sets object type for buffer

    return 0;
}

struct ws_wayland_buffer*
ws_wayland_buffer_new(
    struct wl_resource* r
) {
    struct ws_wayland_buffer* w = calloc(1, sizeof(*w));
    if (!w) {
        return NULL;
    }

    if (ws_wayland_buffer_init(w, r) != 0) {
        goto cleanup;
    }
    w->wl_obj.obj.settings |= WS_OBJECT_HEAPALLOCED;

    return w;

cleanup:
    free(w);
    return NULL;
}

void
ws_wayland_buffer_set_resource(
    struct ws_wayland_buffer* self,
    struct wl_resource* r
) {
    if (wl_shm_buffer_get(r) == NULL) { // we get NULL if it is not a SHM buffer
        self->buf.obj.id = &egl_buffer_type.type;
    } else { // ... kay... it's SHM
        self->buf.obj.id = &shm_buffer_type.type;
    }

    ws_wayland_obj_set_wl_resource(&self->wl_obj, r);
}

struct ws_buffer*
ws_wayland_buffer_get_buffer(
    struct ws_wayland_buffer* self
) {
    return &self->buf;
}


/*
 *
 * Internal implementation
 *
 */

static void*
shm_get_data(
    struct ws_buffer const* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    return wl_shm_buffer_get_data(shm_buffer);
}

static int32_t
shm_get_width(
    struct ws_buffer const* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    if (!res) {
        return 0;
    }
    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    return wl_shm_buffer_get_width(shm_buffer);
}

static int32_t
shm_get_height(
    struct ws_buffer const* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    if (!res) {
        return 0;
    }
    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    return wl_shm_buffer_get_height(shm_buffer);
}

static int32_t
shm_get_stride(
    struct ws_buffer const* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    return wl_shm_buffer_get_stride(shm_buffer);
}

struct ws_egl_fmt const*
shm_get_format(
    struct ws_buffer const* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    return ws_egl_fmt_from_shm_fmt(wl_shm_buffer_get_format(shm_buffer));
}

static int
shm_transfer2texture(
    struct ws_buffer const* self,
    struct ws_texture* texture
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    // get the metadata
    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    struct ws_egl_fmt const* fmt;
    fmt = ws_egl_fmt_from_shm_fmt(wl_shm_buffer_get_format(shm_buffer));

    // bind texture
    ws_texture_bind(texture, GL_TEXTURE_2D);

    // perform the final update
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 wl_shm_buffer_get_stride(shm_buffer)/fmt->bpp,
                 wl_shm_buffer_get_height(shm_buffer), 0,
                 fmt->egl.fmt, fmt->egl.type,
                 wl_shm_buffer_get_data(shm_buffer));

    return eglGetError() == GL_NO_ERROR ? 0 : -1;
}

static void
shm_begin_access(
    struct ws_buffer* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    wl_shm_buffer_begin_access(shm_buffer);
}

static void
shm_end_access(
    struct ws_buffer* self
) {
    // get the resource
    struct ws_wayland_buffer* buf = wl_container_of(self, buf, buf);
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&buf->wl_obj);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(res);
    wl_shm_buffer_end_access(shm_buffer);
}

static int
egl_transfer2texture(
    struct ws_buffer const* self,
    struct ws_texture* texture
) {
    struct ws_wayland_buffer* wbuf = (struct ws_wayland_buffer*) self;
    EGLDisplay dpy = ws_comp_ctx.fb->egl_disp;
    EGLImageKHR gltex = eglCreateImageKHR(dpy, NULL, EGL_WAYLAND_BUFFER_WL,
                                          wbuf->wl_obj.resource, NULL);
    if (gltex == NULL) {
        return -ENOENT;
    }

    ws_texture_bind(texture, GL_TEXTURE_2D);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, gltex);

    return eglGetError() == GL_NO_ERROR ? 0 : -1;
}

static void
egl_begin_access(
    struct ws_buffer* self
) {
    //!< @todo implement
    return;
}

static void
egl_end_access(
    struct ws_buffer* self
) {
    //!< @todo implement
    return;
}

void
ws_wayland_buffer_release(
    struct ws_wayland_buffer* self
) {
    if (!self) {
        ws_log(&log_ctx, LOG_ERR, "We got a NULL surface");
        return;
    }
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(&self->wl_obj);
    wl_buffer_send_release(res);
}

