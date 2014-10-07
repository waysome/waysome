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
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xf86drm.h>

#include "objects/object.h"
#include "compositor/framebuffer_device.h"

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
    .log_callback = NULL,
    .run_callback = NULL
};

struct ws_monitor*
ws_monitor_new(
    void
) {
    struct ws_monitor* tmp = calloc(1, sizeof(*tmp));
    ws_object_init((struct ws_object*)tmp);
    tmp->obj.id = &WS_OBJECT_TYPE_ID_MONITOR;
    return tmp;
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
    if (self->map) {
        munmap(self->map, self->size);
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

static size_t
ws_monitor_hash(
    struct ws_object* obj
) {
    struct ws_monitor* self = (struct ws_monitor*) obj;
    return SIZE_MAX / (self->crtc * self->fb_dev->fd);
}
