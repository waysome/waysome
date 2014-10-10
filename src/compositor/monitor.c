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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <xf86drm.h>

#include "compositor/background_surface.h"
#include "compositor/framebuffer_device.h"
#include "compositor/internal_context.h"
#include "logger/module.h"
#include "objects/object.h"

#include "compositor/monitor.h"

/*
 *
 *  Forward declarations
 *
 */

static bool
ws_monitor_deinit(
    struct ws_object* self
);

static size_t
ws_monitor_hash(
    struct ws_object* self
);

static int
ws_monitor_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/*
 *
 * Interface Implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_MONITOR = {
    .supertype = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr = "ws_monitor",
    .deinit_callback = ws_monitor_deinit,
    .hash_callback = ws_monitor_hash,
    .init_callback = NULL,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = ws_monitor_cmp
};

struct ws_monitor*
ws_monitor_new(
    void
) {
    struct ws_monitor* tmp = calloc(1, sizeof(*tmp));
    ws_object_init((struct ws_object*)tmp);
    tmp->obj.id = &WS_OBJECT_TYPE_ID_MONITOR;
    tmp->obj.settings |= WS_OBJECT_HEAPALLOCED;

    // initialize members
    if (ws_set_init(&tmp->surfaces) < 0) {
        goto cleanup_alloc;
    }

    return tmp;

cleanup_alloc:
    free(tmp);
    return tmp;
}

struct ws_set*
ws_monitor_surfaces(
    struct ws_monitor* self
) {
    return &self->surfaces;
}

/*
 *
 * Internal implementation
 *
 */

static bool
ws_monitor_deinit(
    struct ws_object* obj
) {
    struct ws_monitor* self = (struct ws_monitor*) obj;
    if (self->connected) {
        drmModeSetCrtc(self->fb_dev->fd,
                self->saved_crtc->crtc_id,
                self->saved_crtc->buffer_id,
                self->saved_crtc->x,
                self->saved_crtc->y,
                &self->conn,
                1,
                &self->saved_crtc->mode);
    }
    if (self->buffer->buffer) {
        munmap(self->buffer->buffer, self->buffer->size);
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

    ws_object_deinit((struct ws_object*) &self->surfaces);
    return true;
}

void
ws_monitor_populate_fb(
    struct ws_monitor* self
) {
    struct drm_mode_create_dumb creq; //Create Request
    struct drm_mode_destroy_dumb dreq; //Delete Request
    struct drm_mode_map_dumb mreq; //Memory Request

    if (!self->connected) {
        ws_log(&log_ctx, "Did not create FB for self %d.", self->crtc);
        return;
    }
    memset(&creq, 0, sizeof(creq));
    creq.width = self->buffer->width;
    creq.height = self->buffer->height;
    creq.bpp = 32;
    int ret = drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (ret < 0) {
        ws_log(&log_ctx, "Could not create DUMB BUFFER");
        return;
    }

    self->buffer->stride = creq.pitch;
    self->buffer->size = creq.size;
    self->handle = creq.handle;

    ret = drmModeAddFB(ws_comp_ctx.fb->fd,
            self->buffer->width,
            self->buffer->height, 24, 32,
            self->buffer->stride,
            self->handle, &self->fb);

    if (ret) {
        ws_log(&log_ctx, "Could not add FB of size: %dx%d.",
                creq.width, creq.height);
        goto err_destroy;
    }

    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = self->handle;
    ret = drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret) {
        ws_log(&log_ctx, "Could not allocate enough memory for FB.");
        goto err_fb;
    }

    self->buffer->buffer = mmap(0, self->buffer->size,
            PROT_READ | PROT_WRITE, MAP_SHARED,
            ws_comp_ctx.fb->fd, mreq.offset);

    if (self->buffer->buffer == MAP_FAILED) {
        ws_log(&log_ctx, "Could not MMAP FB");
        goto err_fb;
    }

    memset(self->buffer->buffer, 0, self->buffer->size);

    self->saved_crtc = drmModeGetCrtc(ws_comp_ctx.fb->fd, self->crtc);
    ret = drmModeSetCrtc(ws_comp_ctx.fb->fd, self->crtc, self->fb, 0, 0,
            &self->conn, 1, &self->mode);
    if (ret) {
        ws_log(&log_ctx, "Could not set the CRTC for self %d.",
                self->crtc);
        goto err_fb;
    }

    ws_log(&log_ctx, "Succesfully created Framebuffer");

    return;
err_fb:
    drmModeRmFB(ws_comp_ctx.fb->fd, self->fb);
err_destroy:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = self->handle;
    drmIoctl(ws_comp_ctx.fb->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

    return;
}

static size_t
ws_monitor_hash(
    struct ws_object* obj
) {
    struct ws_monitor* self = (struct ws_monitor*) obj;
    return SIZE_MAX / (self->crtc * self->fb_dev->fd + 1);
}

static int
ws_monitor_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_monitor* mon1 = (struct ws_monitor*) obj1;
    struct ws_monitor* mon2 = (struct ws_monitor*) obj2;

    if (mon1->crtc != mon2->crtc) {
        return (mon1->crtc > mon2->crtc) - (mon1->crtc < mon2->crtc);
    }

    if (mon1->fb_dev->fd != mon2->fb_dev->fd) {
        return (mon1->fb_dev->fd > mon2->fb_dev->fd) -
            (mon1->fb_dev->fd < mon2->fb_dev->fd);
    }
    return 0;
}
