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

// malloc _must_ be included before gbm.h
#include <malloc.h>
#include <gbm.h>

#include "compositor/buffer/egl.h"
#include "compositor/framebuffer_device.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialize an EGL buffer
 */
bool
egl_buffer_deinit(
    struct ws_object* obj
);

/**
 * Get the width of this egl buffer
 */
int32_t
get_width(struct ws_buffer const*);

/**
 * Get the height of this egl buffer
 */
int32_t
get_height(struct ws_buffer const*);
/*
 *
 * Internal constants
 *
 */

ws_buffer_type_id WS_OBJECT_TYPE_ID_EGL_BUFFER = {
    .type = {
        .supertype  = (ws_object_type_id*)&WS_OBJECT_TYPE_ID_BUFFER,
        .typestr    = "ws_egl_buffer",

        .hash_callback = NULL,

        .deinit_callback = NULL,
        .cmp_callback = NULL,
        .uuid_callback = NULL,

        .attribute_table = NULL,
        .function_table = NULL,
    },
    .get_data = NULL,
    .get_width = get_width,
    .get_height = get_height,
    .get_stride = NULL,
    .get_format = NULL,
    .get_bpp = NULL,
    .begin_access = NULL,
    .end_access = NULL,
};


/*
 *
 * Interface implementation
 *
 */

int
ws_egl_buffer_init(
    struct ws_egl_buffer* self,
    struct ws_framebuffer_device* dev,
    EGLConfig egl_conf,
    uint32_t width,
    uint32_t height,
    uint32_t format
) {
    int res = ws_buffer_init(&self->buf);
    if (res < 0) {
        return res;
    }
    self->buf.obj.id = (ws_object_type_id *) &WS_OBJECT_TYPE_ID_EGL_BUFFER;

    self->dev = getref(dev);
    if (!dev) {
        goto cleanup_obj;
    }

    struct gbm_device* gbm_dev;
    gbm_dev = ws_framebuffer_device_get_gbm_dev(dev);
    if (!gbm_dev) {
        goto cleanup_ref;
    }

    self->gbm_surf = gbm_surface_create(gbm_dev, width, height, format,
                                          GBM_BO_USE_SCANOUT |
                                          GBM_BO_USE_RENDERING);
    if (!self->gbm_surf) {
        goto cleanup_ref;
    }

    EGLDisplay egl_disp = ws_framebuffer_device_get_egl_display(dev);
    if (!egl_disp) {
        goto cleanup_gbm;
    }

    self->egl_surf = eglCreateWindowSurface(egl_disp, egl_conf,
                                              self->gbm_surf, NULL);
    if (!self->egl_surf) {
        goto cleanup_gbm;
    }

    return 0;

cleanup_gbm:
    gbm_surface_destroy(self->gbm_surf);

cleanup_ref:
    ws_object_unref((struct ws_object*) self->dev);

cleanup_obj:
    ws_object_deinit((struct ws_object*) self);
    return -1;
}

struct ws_egl_buffer*
ws_egl_buffer_new(
    struct ws_framebuffer_device* dev,
    EGLConfig egl_conf,
    uint32_t width,
    uint32_t height,
    uint32_t format
) {
    struct ws_egl_buffer* retval = calloc(1, sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    if (ws_egl_buffer_init(retval, dev, egl_conf, width, height, format) < 0) {
        free(retval);
        return NULL;
    }
    retval->buf.obj.settings |= WS_OBJECT_HEAPALLOCED;

    return retval;
}

/*
 *
 * Internal implementation
 *
 */

bool
egl_buffer_deinit(
    struct ws_object* obj
) {
    struct ws_egl_buffer* self = (struct ws_egl_buffer*) obj;

    // deinitialize of the EGL surface
    EGLDisplay egl_disp = ws_framebuffer_device_get_egl_display(self->dev);
    if (egl_disp) {
        eglDestroySurface(egl_disp, self->egl_surf);
    }

    // deinitialize the GBM surface and the reference to the framebuffer device
    gbm_surface_destroy(self->gbm_surf);
    ws_object_unref((struct ws_object*) self->dev);

    return true;
}

int32_t
get_width(struct ws_buffer const* _buff) {
    struct ws_egl_buffer* buff = (struct ws_egl_buffer*) _buff;
    EGLDisplay egl_disp = ws_framebuffer_device_get_egl_display(buff->dev);
    EGLint ret;
    eglQuerySurface(egl_disp, buff->egl_surf, EGL_WIDTH, &ret);

    return (int32_t) ret;
}

int32_t
get_height(struct ws_buffer const* _buff) {
    struct ws_egl_buffer* buff = (struct ws_egl_buffer*) _buff;
    EGLDisplay egl_disp = ws_framebuffer_device_get_egl_display(buff->dev);
    EGLint ret;
    eglQuerySurface(egl_disp, buff->egl_surf, EGL_HEIGHT, &ret);

    return (int32_t) ret;
}
