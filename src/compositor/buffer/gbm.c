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
#include <gbm.h>

#include "logger/module.h"
#include "objects/object.h"

#include "compositor/buffer/gbm.h"
#include "compositor/buffer/raw_buffer.h"
#include "compositor/internal_context.h"


/*
 *
 *  Forward declarations
 *
 */

static bool
gbm_buffer_deinit(
    struct ws_object* self
);

/*
 *
 * Interface Implementation
 *
 */

ws_buffer_type_id WS_OBJECT_TYPE_ID_GBM_BUFFER = {
    .type = {
        .supertype  = (ws_object_type_id*) &WS_OBJECT_TYPE_ID_RAW_BUFFER,
        .typestr    = "ws_frame_buffer",

        .hash_callback = NULL,

        .deinit_callback = gbm_buffer_deinit,
        .cmp_callback = NULL,

        .attribute_table = NULL,
        .function_table = NULL,
    },
    .get_data = NULL,
    .get_width = NULL,
    .get_height = NULL,
    .get_stride = NULL,
    .get_format = NULL,
    .transfer2texture = NULL,
    .begin_access = NULL,
    .end_access = NULL,
};

struct ws_gbm_buffer*
ws_gbm_buffer_new(
    struct ws_framebuffer_device* fb_dev,
    int width,
    int height
) {
    struct ws_gbm_buffer* tmp = calloc(1, sizeof(*tmp));
    if (!tmp) {
        return NULL;
    }

    ws_buffer_init(&tmp->obj.obj);
    tmp->obj.obj.obj.settings |= WS_OBJECT_HEAPALLOCED;
    tmp->obj.obj.obj.id = (ws_object_type_id *) &WS_OBJECT_TYPE_ID_GBM_BUFFER;
    tmp->fb_dev = fb_dev;
    struct gbm_device* gbm = ws_framebuffer_device_get_gbm_dev(tmp->fb_dev);

    if (!gbm) {
        goto cleanup_tmp;
    }

    tmp->bo = gbm_bo_create(gbm, width, height,  GBM_BO_FORMAT_XRGB8888,
                            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!tmp->bo) {
        goto cleanup_tmp;
    }

    tmp->handle = gbm_bo_get_handle(tmp->bo).u32;
    tmp->obj.stride = gbm_bo_get_stride(tmp->bo);
    tmp->obj.width = width;
    tmp->obj.height = height;

    drmModeAddFB(fb_dev->fd, width, height, 24, 32, tmp->obj.stride,
                 tmp->handle, &tmp->fb);

    eglMakeCurrent(fb_dev->egl_disp, fb_dev->egl_conf, EGL_NO_SURFACE, NULL);


    return tmp;
cleanup_tmp:
    free(tmp);
    return NULL;
}

/*
 *
 * Internal implementation
 *
 */

static bool
gbm_buffer_deinit(
    struct ws_object* obj
) {
    return true;
}
