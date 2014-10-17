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

#include "compositor/buffer/image_buffer.h"
#include "compositor/framebuffer_device.h"
#include "compositor/internal_context.h"
#include "compositor/monitor.h"
#include "logger/module.h"
#include "objects/object.h"

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

    if (ws_set_init(&tmp->modes) < 0) {
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

    ws_object_deinit((struct ws_object*) &self->surfaces);
    return true;
}

void
ws_monitor_populate_fb(
    struct ws_monitor* self
) {

    if (!self->connected) {
        ws_log(&log_ctx, LOG_DEBUG,
                "Did not create FB for self %d.", self->crtc);
        return;
    }
    self->buffer = ws_frame_buffer_new(
            self->fb_dev,
            self->current_mode->mode.hdisplay,
            self->current_mode->mode.vdisplay
    );
    if (!self->buffer) {
        ws_log(&log_ctx, LOG_CRIT, "Could not create Framebuffer");
    }
    self->saved_crtc = drmModeGetCrtc(ws_comp_ctx.fb->fd, self->crtc);

    int ret = drmModeSetCrtc(ws_comp_ctx.fb->fd, self->crtc, self->buffer->fb,
            0, 0, &self->conn, 1, &self->current_mode->mode);
    if (ret) {
        ws_log(&log_ctx, LOG_ERR, "Could not set the CRTC for self %d.",
                self->crtc);
    }
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

    if (mon1->id != mon2->id) {
        return (mon1->id > mon2->id) - (mon1->id < mon2->id);
    }

    if (mon1->fb_dev->fd != mon2->fb_dev->fd) {
        return (mon1->fb_dev->fd > mon2->fb_dev->fd) -
            (mon1->fb_dev->fd < mon2->fb_dev->fd);
    }
    return 0;
}

void
ws_monitor_set_mode_with_id(
    struct ws_monitor* self,
    int id
) {
    struct ws_monitor_mode mode;
    memset(&mode, 0, sizeof(mode));
    mode.obj.id = &WS_OBJECT_TYPE_ID_MONITOR_MODE;
    mode.id = id;
    self->current_mode =
        (struct ws_monitor_mode*) ws_set_get(
                &self->modes,
                (struct ws_object*)&mode);
}
