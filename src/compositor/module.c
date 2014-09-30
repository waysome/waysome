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
#include <unistd.h>
#include <xf86drm.h>

#include "util/cleaner.h"
#include "compositor/internal_context.h"


struct ws_compositor_context ws_comp_ctx;


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

    while(it) {
        struct ws_monitor* next = it->next;
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
        enc = drmModeGetEncoder(ws_comp_ctx.fb.fd, conn->encoder_id);
    } else {
        enc = NULL;
    }

    // If we do have an encoder, we check that noone else uses this crtc
    if (enc) {
        if (enc->crtc_id) {
            crtc = enc->crtc_id;

            if (find_connector_with_crtc(crtc) != NULL) {
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
            //!< @todo: Log Error!
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
            if (find_connector_with_crtc(crtc) != NULL) {
                drmModeFreeEncoder(enc);
                connector->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    //!< @todo: Log Error! No CRTC FOUND!!!
    return -ENOENT;
}

static int
populate_connectors(void) {
    drmModeRes* res;
    drmModeConnector* conn;
    struct ws_monitor** connector = &ws_comp_ctx.conns;

    res = drmModeGetResources(ws_comp_ctx.fb.fd);
    if (!res) {
        //!< @todo: Log Error
        return -ENOENT;
    }

    // Let's go through all connectors (outputs)
    int i = res->count_connectors;
    while(i--) {
        conn = drmModeGetConnector(ws_comp_ctx.fb.fd, res->connectors[i]);
        if (!conn) {
            //!< @todo: Log Error
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
            //!< @todo: Log Unused
            (*connector)->connected = 0;
            continue;
        }

        if (conn->count_modes == 0) {
            //!< @todo: Log No Valid Modes
            (*connector)->connected = 0;
            continue;
        }

        //!< @todo: Do not just take the biggest mode available
        memcpy(&(*connector)->mode, &conn->modes[0],
                sizeof((*connector)->mode));

        (*connector)->width = conn->modes[0].hdisplay;
        (*connector)->height = conn->modes[0].vdisplay;


        if (find_crtc(res, conn, *connector) < 0) {
            //!< @todo: Log error about not finding crtcs
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
        //!< @todo: Log Error!
        return -ENOENT;
    }

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        //!< @todo: Log Error!
        close(fd);
        return -EOPNOTSUPP;
    }
    ws_comp_ctx.fb.path = path;
    ws_comp_ctx.fb.fd = fd;
    return 0;
}

