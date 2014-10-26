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
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "logger/module.h"
#include "objects/object.h"

#include "compositor/buffer/frame.h"
#include "compositor/buffer/image.h"
#include "compositor/internal_context.h"


/*
 *
 *  Forward declarations
 *
 */

static bool
frame_buffer_deinit(
    struct ws_object* self
);

/*
 *
 * Interface Implementation
 *
 */

ws_buffer_type_id WS_OBJECT_TYPE_ID_FRAME_BUFFER = {
    .type = {
        .supertype  = (ws_object_type_id*)&WS_OBJECT_TYPE_ID_IMAGE_BUFFER,
        .typestr    = "ws_frame_buffer",

        .hash_callback = NULL,

        .deinit_callback = frame_buffer_deinit,
        .dump_callback = NULL,
        .run_callback = NULL,
        .cmp_callback = NULL,
    },
    .get_data = NULL,
    .get_width = NULL,
    .get_height = NULL,
    .get_stride = NULL,
    .get_format = NULL,
    .get_bpp = NULL,
    .begin_access = NULL,
    .end_access = NULL,
};

struct ws_frame_buffer*
ws_frame_buffer_new(
    struct ws_framebuffer_device* fb_dev,
    int width,
    int height
) {
    struct ws_frame_buffer* tmp = calloc(1, sizeof(*tmp));
    ws_buffer_init(&tmp->obj.obj);
    tmp->obj.obj.obj.settings |= WS_OBJECT_HEAPALLOCED;
    tmp->obj.obj.obj.id = (ws_object_type_id *)&WS_OBJECT_TYPE_ID_FRAME_BUFFER;
    tmp->fb_dev = fb_dev;

    struct drm_mode_create_dumb creq; //Create Request
    struct drm_mode_destroy_dumb dreq; //Delete Request
    struct drm_mode_map_dumb mreq; //Memory Request

    memset(&creq, 0, sizeof(creq));
    creq.width = width;
    creq.height = height;
    creq.bpp = 32;
    int ret = drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not create DUMB BUFFER");
        goto err_destroy;
    }

    tmp->obj.width = creq.width;
    tmp->obj.height = creq.height;
    tmp->obj.stride = creq.pitch;
    tmp->obj.size = creq.size;
    tmp->handle = creq.handle;

    ret = drmModeAddFB(ws_comp_ctx.fb->fd,
            tmp->obj.width,
            tmp->obj.height, 24, 32,
            tmp->obj.stride,
            tmp->handle, &tmp->fb);

    if (ret) {
        ws_log(&log_ctx, LOG_CRIT, "Could not add FB of size: %dx%d.",
                creq.width, creq.height);
        goto err_destroy;
    }

    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = tmp->handle;
    ret = drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        ws_log(&log_ctx, LOG_CRIT, "Could not allocate enough memory for FB.");
        goto err_fb;
    }

    tmp->obj.buffer = mmap(0, tmp->obj.size,
            PROT_READ | PROT_WRITE, MAP_SHARED,
            ws_comp_ctx.fb->fd, mreq.offset);

    if (tmp->obj.buffer == MAP_FAILED) {
        ws_log(&log_ctx, LOG_CRIT, "Could not MMAP FB");
        goto err_fb;
    }

    memset(tmp->obj.buffer, 0, tmp->obj.size);

    ws_log(&log_ctx, LOG_DEBUG, "Succesfully created Frameobj");

    return tmp;
err_fb:
    drmModeRmFB(ws_comp_ctx.fb->fd, tmp->fb);
err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = tmp->handle;
    drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    free(tmp);
    return NULL;
}

/*
 *
 * Internal implementation
 *
 */

static bool
frame_buffer_deinit(
    struct ws_object* obj
) {
    struct ws_frame_buffer* self = (struct ws_frame_buffer*) obj;
    if (self->obj.buffer) {
        munmap(self->obj.buffer, self->obj.size);
    }

    if (self->fb) {
        drmModeRmFB(self->fb_dev->fd, self->fb);
    }

    if (self->handle) {
        struct drm_mode_destroy_dumb dreq;
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = self->handle;
        drmIoctl(self->fb_dev->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }
    ws_object_unref((struct ws_object*)self->fb_dev);
    return true;
}
