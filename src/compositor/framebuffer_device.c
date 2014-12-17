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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xf86drm.h>

#include <gbm.h>

#include "compositor/framebuffer_device.h"
#include "compositor/internal_context.h"
#include "logger/module.h"
#include "objects/object.h"

static struct ws_logger_context log_ctx = {
    .prefix = "[Compositor/FBDevice] "
};

/*
 *
 *  Forward declarations
 *
 */

static bool
device_deinit(
    struct ws_object* self
);

static size_t
device_hash(
    struct ws_object* obj
);

static int
device_cmp(
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
    .deinit_callback = device_deinit,
    .hash_callback = device_hash,
    .cmp_callback = device_cmp,

    .attribute_table = NULL,
    .function_table = NULL,
};

struct ws_framebuffer_device*
ws_framebuffer_device_new(
    char* path
) {
    struct ws_framebuffer_device* tmp = calloc(1, sizeof(*tmp));
    ws_object_init((struct ws_object*) tmp);
    tmp->obj.id = &WS_OBJECT_TYPE_ID_FRAMEBUFFER_DEVICE;
    tmp->obj.settings |= WS_OBJECT_HEAPALLOCED;
    tmp->fd = open(path, O_RDWR | O_CLOEXEC);

    if (tmp->fd < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not open: '%s'.", path);
        free(tmp);
        return NULL;
    }

    uint64_t has_dumb;
    if (drmGetCap(tmp->fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        ws_log(&log_ctx, LOG_CRIT, "File %s has no DUMB BUFFER cap. ", path);
        close(tmp->fd);
        free(tmp);
        return NULL;
    }
    tmp->path = strdup(path);

    tmp->gbm_dev = NULL;
    tmp->egl_disp = NULL;

    return tmp;
}

struct gbm_device*
ws_framebuffer_device_get_gbm_dev(
    struct ws_framebuffer_device* self
) {
    if (self->gbm_dev) {
        return self->gbm_dev;
    }

    self->gbm_dev = gbm_create_device(self->fd);
    return self->gbm_dev;
}

EGLDisplay
ws_framebuffer_device_get_egl_display(
    struct ws_framebuffer_device* self
) {
    if (self->egl_disp) {
        return self->egl_disp;
    }

    // get the GBM device to base the EGL stuff on
    struct gbm_device* gbm_dev = ws_framebuffer_device_get_gbm_dev(self);
    if (!gbm_dev) {
        return NULL;
    }

    // get and initialize the display
    EGLDisplay disp = eglGetDisplay(gbm_dev);

    if (!eglInitialize(disp, NULL, NULL)) {
        return NULL;
    }

    EGLint egl_config_attribs[] = {
        EGL_BUFFER_SIZE,        32,
        EGL_DEPTH_SIZE,         EGL_DONT_CARE,
        EGL_STENCIL_SIZE,       EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
        EGL_NONE,
    };

    EGLint num_configs;
    int ret = eglGetConfigs(disp, NULL, 0, &num_configs);
    if (!ret) {
        ws_log(&log_ctx, LOG_ERR, "Could not get configs for egl display");
        return NULL;
    }

    // This is a weird moment where we use /parts/ of the array, but can't free
    // all of it!
    EGLConfig* configs = calloc(num_configs, sizeof(*configs));

    ret = eglChooseConfig(disp, egl_config_attribs, configs, num_configs,
                            &num_configs);
    if (!ret || !num_configs) {
        ws_log(&log_ctx, LOG_ERR, "Could not get configs for egl display");
        return NULL;
    }

    EGLConfig effective_config = NULL;

    for (int i = 0; i < num_configs; ++i) {
        EGLint gbm_format;

        ret = eglGetConfigAttrib(disp, configs[i], EGL_NATIVE_VISUAL_ID,
                                  &gbm_format);

        if (!ret) {
            ws_log(&log_ctx, LOG_ERR, "Could not get config attributes");
            return NULL;
        }

        if (gbm_format == GBM_FORMAT_XRGB8888) {
            effective_config = configs[i];
            break;
        }
    }

    self->egl_conf = effective_config;
    self->egl_ctx  = eglCreateContext(disp, effective_config, NULL, NULL);

    free(configs);
    return self->egl_disp = disp;
}

/*
 *
 * Internal implementation
 *
 */

static bool
device_deinit(
    struct ws_object* obj
) {
    struct ws_framebuffer_device* self = (struct ws_framebuffer_device*) obj;

    // destruct gbm device
    if (self->gbm_dev) {
        gbm_device_destroy(self->gbm_dev);
    }

    if (self->fd > 0) {
        close(self->fd);
    }
    free(self->path);
    return true;
}

static size_t
device_hash(
    struct ws_object* obj
) {
    struct ws_framebuffer_device* self = (struct ws_framebuffer_device*) obj;
    return SIZE_MAX / self->fd;
}

static int
device_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_framebuffer_device* dev1 = (struct ws_framebuffer_device*) obj1;
    struct ws_framebuffer_device* dev2 = (struct ws_framebuffer_device*) obj2;
    // It's short signum function
    return (dev1->fd > dev2->fd) - (dev1->fd < dev2->fd);
}
