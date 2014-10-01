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

#include "compositor/module.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xf86drm.h>

#include "util/cleaner.h"
#include "logger/module.h"
#include "compositor/internal_context.h"

struct ws_compositor_context ws_comp_ctx;
static struct ws_logger_context log_ctx = { "[Compositor] " };


/**
 * Find the connector associated with a crtc id
 *
 * This function iterates over all connectors, searching for a connector with
 * the id specified.
 *
 * @return the connector for a crtc or NULL, if no such connector exists.
 */
static struct ws_monitor*
find_connector_with_crtc(
    int crtc
);


/**
 * Find a suitable connector and return it's id
 *
 * This function tries to find a connector which is
 *  - free
 *  - matches the encoding
 *  - can be used to access the monitor passed to the function
 * and sets the crtc for a monitor
 *
 * @returns 0 or a negative error value (-ENOENT)
 */
static int
find_crtc(
    drmModeRes* res,
    drmModeConnector* conn,
    struct ws_monitor* connector
);

/**
 * Find all connectors on the given framebuffer device
 *
 * The connectors found are added to the given compositor context directly.
 *
 * @return 0 on success, a negative error code otherwise (-ENOENT)
 */
static int
populate_connectors(void);

/**
 * Create framebuffers for all connected connectors
 *
 * @return 0 on success, a negative error code otherwise (-ENOENT)
 */
static int
populate_framebuffers(void);


/**
 * Get a Framebuffer Device provided by DRM using the given path
 *
 * This is the graphic card device that is given by the kernel. It loads it
 * and populates the given framebuffer with the information found.
 *
 * @return 0 on success, a negative error code otherwise (-ENOENT)
 */
static int
get_framebuffer_device(
    const char* path
);


/**
 * Deinitialise the compositor
 *
 * This function deinitialises the compositor.
 * It does not take care of inter-module dependency nor does it check whether
 * it was called before, unlike ws_compositor_init().
 *
 * @warning call this function only once
 */
static void
ws_compositor_deinit(
    void* dummy
);


/*
 *
 * Interface implementation
 *
 */

int
ws_compositor_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }

    ws_log(&log_ctx, "Starting initialization of the Compositor.");

    ws_cleaner_add(ws_compositor_deinit, NULL);
    int retval;

    retval = get_framebuffer_device("/dev/dri/card0");
    if (retval < 0) {
        return retval;
    }

    retval = populate_connectors();
    if (retval < 0) {
        return retval;
    }

    retval = populate_framebuffers();
    if (retval < 0) {
        return retval;
    }

    retval =
    //!< @todo: create a framebuffer for each connector

    //!< @todo: prelimary: preload a PNG from a hardcoded path

    //!< @todo: prelimary: blit the preloaded PNG on each of the frame buffers

    is_init = true;
    return 0;
}


/*
 *
 * Internal implementation
 *
 */


static void
ws_compositor_deinit(
    void* dummy
) {

    if (ws_comp_ctx.fb.fd >= 0) {
        close(ws_comp_ctx.fb.fd);
    }

    struct ws_monitor* it = ws_comp_ctx.conns;

    struct drm_mode_destroy_dumb dreq;
    while(it) {
        struct ws_monitor* next = it->next;

        drmModeSetCrtc(ws_comp_ctx.fb.fd,
                it->saved_crtc->crtc_id,
                it->saved_crtc->buffer_id,
                it->saved_crtc->x,
                it->saved_crtc->y,
                &it->conn,
                1,
                &it->saved_crtc->mode);
        if (it->map) {
            munmap(it->map, it->size);
        }

        drmModeRmFB(ws_comp_ctx.fb.fd, it->fb);

        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = it->handle;
        drmIoctl(ws_comp_ctx.fb.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        free(it);
        it = next;
    }

    //!< @todo: free all of the framebuffers

    //!< @todo: prelimary: free the preloaded PNG
}

static struct ws_monitor*
find_connector_with_crtc(
        int crtc
) {
    for (struct ws_monitor* iter = ws_comp_ctx.conns; iter; iter = iter->next) {
        if (iter->crtc == (uint32_t)crtc) {
            return iter;
        }
    }
    return NULL;
}

static int
find_crtc(
    drmModeRes* res,
    drmModeConnector* conn,
    struct ws_monitor* connector
) {
    drmModeEncoder* enc;
    int32_t crtc;

    // We check if we already have found a suitable encoder
    if (conn->encoder_id) {
        ws_log(&log_ctx, "Found an existing encoder for monitor: %dx%d",
                connector->width, connector->height);
        enc = drmModeGetEncoder(ws_comp_ctx.fb.fd, conn->encoder_id);
    } else {
        ws_log(&log_ctx, "Found no existing encoder for monitor: %dx%d",
                connector->width, connector->height);
        enc = NULL;
    }

    // If we do have an encoder, we check that noone else uses this crtc
    if (enc) {
        if (enc->crtc_id) {
            ws_log(&log_ctx, "There seems to be a crtc on here.");
            crtc = enc->crtc_id;

            if (find_connector_with_crtc(crtc) != NULL) {
                ws_log(&log_ctx, "There was a crtc! Setting it.");
                drmModeFreeEncoder(enc);
                connector->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }
    // There is no encoder+crtc pair! We go through /all/ the encoders now
    for (int i = 0; i < conn->count_encoders; ++i) {
        enc = drmModeGetEncoder(ws_comp_ctx.fb.fd, conn->encoders[i]);

        if (!enc) {
            ws_log(&log_ctx, "Could not get Encoder.");
            continue;
        }

        for( int j = 0; j < res->count_crtcs; ++j) {
            // Check if this encoding supports with all the crtcs
            if(!(enc->possible_crtcs & (1 << j))) {
                continue;
            }

            // Check noone else uses it!
            crtc = res->crtcs[j];

            // Looks like we found one! Return!
            if (find_connector_with_crtc(crtc) == NULL) {
                drmModeFreeEncoder(enc);
                ws_log(&log_ctx, "Found a CRTC! Saving");
                connector->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    ws_log(&log_ctx, "Could not find suitable Encoder for crtc with dim: %dx%d.",
            connector->width, connector->height);
    return -ENOENT;
}

static int
populate_connectors(void) {
    drmModeRes* res;
    drmModeConnector* conn;
    struct ws_monitor** connector = &ws_comp_ctx.conns;

    res = drmModeGetResources(ws_comp_ctx.fb.fd);
    if (!res) {
        ws_log(&log_ctx, "Could not get Resources for: %s.",
                ws_comp_ctx.fb.path);
        return -ENOENT;
    }

    // Let's go through all connectors (outputs)
    int i = res->count_connectors;
    while(i--) {
        conn = drmModeGetConnector(ws_comp_ctx.fb.fd, res->connectors[i]);
        if (!conn) {
            ws_log(&log_ctx, "Could not get connector for: %s",
                    ws_comp_ctx.fb.path);
            continue;
        }
        if (*connector) {
            (*connector)->next = calloc(1, sizeof(**connector));
            *connector = (*connector)->next;
        } else {
            *connector = calloc(1, sizeof(**connector));
        }
        (*connector)->conn = conn->connector_id;

        if (conn->connection != DRM_MODE_CONNECTED) {
            ws_log(&log_ctx, "Found unused connector");
            (*connector)->connected = 0;
            continue;
        }

        if (conn->count_modes == 0) {
            ws_log(&log_ctx, "No valid modes for Connector %d.",
                    conn->connector_id);
            (*connector)->connected = 0;
            continue;
        }

        ws_log(&log_ctx, "Found a valid connector with %d modes.",
                conn->count_modes);

        //!< @todo: Do not just take the biggest mode available
        memcpy(&(*connector)->mode, &conn->modes[0],
                sizeof((*connector)->mode));

        (*connector)->width = conn->modes[0].hdisplay;
        (*connector)->height = conn->modes[0].vdisplay;

        ws_log(&log_ctx, "Found a valid connector with %dx%d dimensions.",
                (*connector)->width, (*connector)->height);

        if (find_crtc(res, conn, *connector) < 0) {
            ws_log(&log_ctx, "No valid crtcs found");
            (*connector)->connected = 0;
            continue;
        }
        (*connector)->connected = 1;
    }
    return 0;
}

static int
get_framebuffer_device(
    const char* path
) {
    int fd = open(path, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        ws_log(&log_ctx, "Could not open: '%s'.", path);
        return -ENOENT;
    }

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        ws_log(&log_ctx, "File %s has no DUMB BUFFER cap. ", path);
        close(fd);
        return -EOPNOTSUPP;
    }
    ws_comp_ctx.fb.path = path;
    ws_comp_ctx.fb.fd = fd;
    return 0;
}

static int
populate_framebuffers(
    void
) {

    struct drm_mode_create_dumb creq; //Create Request
    struct drm_mode_destroy_dumb dreq; //Delete Request
    struct drm_mode_map_dumb mreq; //Memory Request

    for (struct ws_monitor* iter = ws_comp_ctx.conns; iter; iter = iter->next) {
        if (!iter->connected) {
            continue;
        }
        memset(&creq, 0, sizeof(creq));
        creq.width = iter->width;
        creq.height = iter->height;
        creq.bpp = 32;
        int ret = drmIoctl(ws_comp_ctx.fb.fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
        if (ret < 0) {
            ws_log(&log_ctx, "Could not create DUMB BUFFER");
            continue;
        }

        iter->stride = creq.pitch;
        iter->size = creq.size;
        iter->handle = creq.handle;

        ret = drmModeAddFB(ws_comp_ctx.fb.fd, iter->width, iter->height, 24, 32,
                iter->stride, iter->handle, &iter->fb);

        if (ret) {
            ws_log(&log_ctx, "Could not add FB of size: %dx%d.",
                    creq.width, creq.height);
            goto err_destroy;
        }

        memset(&mreq, 0, sizeof(mreq));
        mreq.handle = iter->handle;
        ret = drmIoctl(ws_comp_ctx.fb.fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
        if (ret) {
            ws_log(&log_ctx, "Could not allocate enough memory for FB.");
            goto err_fb;
        }

        iter->map = mmap(0, iter->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                ws_comp_ctx.fb.fd, mreq.offset);

        if (iter->map == MAP_FAILED) {
            ws_log(&log_ctx, "Could not MMAP FB");
            goto err_fb;
        }

        memset(iter->map, 0, iter->size);

        iter->saved_crtc = drmModeGetCrtc(ws_comp_ctx.fb.fd, iter->crtc);
        ret = drmModeSetCrtc(ws_comp_ctx.fb.fd, iter->crtc, iter->fb, 0, 0,
                &iter->conn, 1, &iter->mode);
        if (ret) {
            ws_log(&log_ctx, "Could not set the CRTC.");
            goto err_fb;
        }

        continue;
err_fb:
        drmModeRmFB(ws_comp_ctx.fb.fd, iter->fb);
err_destroy:
        memset(&dreq, 0, sizeof(dreq));
        dreq.handle = iter->handle;
        drmIoctl(ws_comp_ctx.fb.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }
    return 0;
}
