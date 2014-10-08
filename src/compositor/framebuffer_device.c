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
#include <unistd.h>
#include <unistd.h>
#include <xf86drm.h>

#include "logger/module.h"
#include "objects/object.h"

#include "compositor/framebuffer_device.h"
#include "compositor/internal_context.h"


/*
 *
 *  Forward declarations
 *
 */

static bool
ws_framebuffer_device_deinit(
    struct ws_object* self
);

static size_t
ws_framebuffer_device_hash(
    struct ws_object* obj
);

static int
ws_framebuffer_device_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/*
 *
 * Interface Implementation
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_FRAMEBUFFER_DEVICE = {
    .supertype = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr = "ws_framebuffer_device",
    .deinit_callback = ws_framebuffer_device_deinit,
    .hash_callback = ws_framebuffer_device_hash,
    .init_callback = NULL,
    .log_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = ws_framebuffer_device_cmp
};

struct ws_framebuffer_device*
ws_framebuffer_device_new(
    char* path
) {
    struct ws_framebuffer_device* tmp = calloc(1, sizeof(*tmp));
    ws_object_init((struct ws_object*)tmp);
    tmp->obj.id = &WS_OBJECT_TYPE_ID_FRAMEBUFFER_DEVICE;
    tmp->fd = open(path, O_RDWR | O_CLOEXEC);

    if (tmp->fd < 0) {
        ws_log(&log_ctx, "Could not open: '%s'.", path);
        free(tmp);
        return NULL;
    }

    uint64_t has_dumb;
    if (drmGetCap(tmp->fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        ws_log(&log_ctx, "File %s has no DUMB BUFFER cap. ", path);
        close(tmp->fd);
        free(tmp);
        return NULL;
    }
    tmp->path = strdup(path);
    return tmp;
}

/*
 *
 * Internal implementation
 *
 */

static bool
ws_framebuffer_device_deinit(
    struct ws_object* obj
) {
    struct ws_framebuffer_device* self = (struct ws_framebuffer_device*) obj;
    if (self->fd > 0) {
        close(self->fd);
    }
    free(self->path);
    return true;
}

static size_t
ws_framebuffer_device_hash(
    struct ws_object* obj
) {
    struct ws_framebuffer_device* self = (struct ws_framebuffer_device*) obj;
    return SIZE_MAX / self->fd;
}

static int
ws_framebuffer_device_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_framebuffer_device* dev1 = (struct ws_framebuffer_device*) obj1;
    struct ws_framebuffer_device* dev2 = (struct ws_framebuffer_device*) obj2;
    // It's short signum function
    return (dev1->fd > dev2->fd) - (dev1->fd < dev2->fd);
}
