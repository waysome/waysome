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
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <wayland-server.h>
#include <xf86drm.h>

#include "compositor/buffer/image_buffer.h"
#include "compositor/cursor.h"
#include "compositor/internal_context.h"
#include "compositor/monitor.h"
#include "compositor/monitor_mode.h"
#include "compositor/wayland/seat.h"
#include "compositor/wayland/wayland_compositor.h"
#include "compositor/wayland/wayland_shell.h"
#include "logger/module.h"
#include "util/cleaner.h"
#include "util/wayland.h"

struct ws_compositor_context ws_comp_ctx;
struct ws_logger_context log_ctx = { .prefix = "[Compositor] " };


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
populate_framebuffers(
    void* dummy,
    void const* mon
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

static int
blit_duck_on_monitor(
    void* dummy,
    void const* mon
);

static int
set_monitor_modes(
    void* dummy,
    void const* mon
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

    ws_log(&log_ctx, LOG_DEBUG, "Initing monitor set");
    ws_set_init(&ws_comp_ctx.monitors);

    ws_log(&log_ctx, LOG_DEBUG, "Starting initialization of the Compositor.");

    ws_cleaner_add(ws_compositor_deinit, NULL);
    int retval;

    ws_comp_ctx.fb = ws_framebuffer_device_new("/dev/dri/card0");

    retval = populate_connectors();
    if (retval < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Populate Connectors failed");
        return retval;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Setting modes");
    retval = ws_set_select(&ws_comp_ctx.monitors, NULL, NULL,
            set_monitor_modes, NULL);
    if (retval < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Setting modes failed");
        return retval;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Populating monitors with Framebuffers");
    retval = ws_set_select(&ws_comp_ctx.monitors, NULL, NULL,
            populate_framebuffers, NULL);
    if (retval < 0) {
        ws_log(&log_ctx, LOG_DEBUG, "Populate Framebuffers failed");
        return retval;
    }

    //!< @todo: Port to buffer code once it is implemented
    struct ws_image_buffer* duck = ws_image_buffer_from_png("duck.png");

    ws_log(&log_ctx, LOG_DEBUG, "Starting blitting");
    ws_set_select(&ws_comp_ctx.monitors, NULL, NULL,
            blit_duck_on_monitor, duck);


    // initialize wayland specific stuff
    struct wl_display* display = ws_wayland_acquire_display();
    if (!display) {
        return -1;
    }

    // try to initialize the shared memory subsystem
    if (wl_display_init_shm(display) != 0) {
        goto cleanup_display;
    }

    ws_wayland_release_display();

    retval = ws_wayland_compositor_init();
    if (retval < 0) {
        return retval;
    }

    retval = ws_wayland_shell_init();
    if (retval < 0) {
        return retval;
    }

    retval = ws_wayland_seat_init();
    if (retval < 0) {
        return retval;
    }

    //!< @todo: Make this more abstract
    struct ws_image_buffer* cursor =
        ws_image_buffer_from_png("share/waysome/cursor.png");

    ws_comp_ctx.cursor = ws_cursor_new(ws_comp_ctx.fb, cursor);
    struct ws_monitor* any =
        (struct ws_monitor*) ws_set_select_any(&ws_comp_ctx.monitors);
    ws_cursor_set_monitor(ws_comp_ctx.cursor, any);
    ws_cursor_redraw(ws_comp_ctx.cursor);

    is_init = true;
    return 0;

    // cleanup wayland display

cleanup_display:
    ws_wayland_release_display();
    return -1;
}


/*
 *
 * Internal implementation
 *
 */


static int
blit_duck_on_monitor(
    void* img,
    void const* mon
) {
    struct ws_monitor* monitor = (struct ws_monitor*)mon;
    struct ws_image_buffer* duck = (struct ws_image_buffer*)img;

    if (!monitor->connected) {
        ws_log(&log_ctx, LOG_DEBUG, "Monitor %d is not connected",
                monitor->crtc);
        return 0;
    }
    if (!monitor->current_mode) {
        ws_log(&log_ctx, LOG_DEBUG, "Not blitting into crtc %d", monitor->crtc);
        return 0;
    }
    ws_log(&log_ctx, LOG_DEBUG, "Copying into monitor with name: %s",
            monitor->current_mode->mode.name);
    ws_buffer_blit((struct ws_buffer*)monitor->buffer,
            (struct ws_buffer*)duck);
    return 0;
}

static void
ws_compositor_deinit(
    void* dummy
) {

    if (ws_comp_ctx.fb->fd >= 0) {
        close(ws_comp_ctx.fb->fd);
    }

    if (ws_comp_ctx.cursor) {
        ws_cursor_unset(ws_comp_ctx.cursor);
        ws_object_deinit((struct ws_object*) ws_comp_ctx.cursor);
    }

    //!< @todo: free all of the framebuffers

    //!< @todo: prelimary: free the preloaded PNG
}

static int
set_monitor_modes(
    void* dummy,
    void const* mon
) {
    struct ws_monitor* monitor = (struct ws_monitor*)mon;

    if (!monitor->connected) {
        return 0;
    }
    if (monitor->mode_count < 1) {
        ws_log(&log_ctx, LOG_ERR, "No modes for a connected monitor.");
        return 1;
    }
    // Set to the biggest mode
    ws_monitor_set_mode_with_id(monitor, monitor->mode_count - 1);
    if (monitor->current_mode) {
        ws_log(&log_ctx, LOG_DEBUG,
                "Found a valid connector with %dx%d dimensions.",
                monitor->current_mode->mode.hdisplay,
                monitor->current_mode->mode.vdisplay);
    } else {
        ws_log(&log_ctx, LOG_DEBUG,
                "Mode setting failed on crtc %d.", monitor->crtc);
        return 1;
    }
    return 0;
}

static struct ws_monitor*
find_connector_with_crtc(
        int crtc
) {
    struct ws_monitor dummy;
    memset(&dummy, 0, sizeof(dummy));
    dummy.obj.id = &WS_OBJECT_TYPE_ID_MONITOR;
    dummy.crtc = crtc;
    dummy.fb_dev = ws_comp_ctx.fb;
    return (struct ws_monitor*)ws_set_get(&ws_comp_ctx.monitors,
            (struct ws_object*)&dummy);
}

static int
find_crtc(
    drmModeRes* res,
    drmModeConnector* conn,
    struct ws_monitor* connector
) {
    drmModeEncoder* enc;
    int32_t crtc;

    // If we do have an encoder, we check that noone else uses this crtc
    // if (enc) {
    //     if (enc->crtc_id) {
    //         ws_log(&log_ctx, LOG_DEBUG, "There seems to be a crtc on here.");
    //         crtc = enc->crtc_id;

    //         if (find_connector_with_crtc(crtc) != NULL) {
    //             ws_log(&log_ctx, LOG_DEBUG, "There was a crtc! Setting it.");
    //             drmModeFreeEncoder(enc);
    //             connector->crtc = crtc;
    //             return 0;
    //         }
    //     }

    //     drmModeFreeEncoder(enc);
    // }
    // There is no encoder+crtc pair! We go through /all/ the encoders now
    for (int i = 0; i < conn->count_encoders; ++i) {
        enc = drmModeGetEncoder(ws_comp_ctx.fb->fd, conn->encoders[i]);

        if (!enc) {
            ws_log(&log_ctx, LOG_DEBUG, "Could not get Encoder.");
            continue;
        }

        ws_log(&log_ctx, LOG_DEBUG, "Found %d crtcs", res->count_crtcs);
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
                ws_log(&log_ctx, LOG_DEBUG, "Found a CRTC! Saving");
                connector->crtc = crtc;
                return 0;
            }
        }

        drmModeFreeEncoder(enc);
    }

    ws_log(&log_ctx, LOG_DEBUG,
            "Could not find suitable Encoder for crtc with dim: %dx%d.",
            ws_buffer_width((struct ws_buffer*)connector->buffer),
            ws_buffer_height((struct ws_buffer*)connector->buffer));
    return -ENOENT;
}

static int
populate_connectors(void) {
    drmModeRes* res;
    drmModeConnector* conn;
    // struct ws_monitor* connector = ws_comp_ctx.conns;

    res = drmModeGetResources(ws_comp_ctx.fb->fd);
    if (!res) {
        ws_log(&log_ctx, LOG_DEBUG, "Could not get Resources for: %s.",
                ws_comp_ctx.fb->path);
        return -ENOENT;
    }

    // Let's go through all connectors (outputs)
    int i = res->count_connectors;
    ws_log(&log_ctx, LOG_DEBUG, "Found a max of %d connectors.", i);
    while(i--) {
        conn = drmModeGetConnector(ws_comp_ctx.fb->fd, res->connectors[i]);
        if (!conn) {
            ws_log(&log_ctx, LOG_DEBUG, "Could not get connector for: %s",
                    ws_comp_ctx.fb->path);
            goto insert;
        }
        struct ws_monitor* new_monitor = ws_monitor_new();
        new_monitor->conn = conn->connector_id;
        new_monitor->fb_dev = ws_comp_ctx.fb;

        if (conn->connection != DRM_MODE_CONNECTED) {
            ws_log(&log_ctx, LOG_DEBUG, "Found unused connector %d", i);
            new_monitor->connected = false;
            goto insert;
        }

        if (conn->count_modes == 0) {
            ws_log(&log_ctx, LOG_DEBUG, "No valid modes for Connector %d.",
                    conn->connector_id);
            new_monitor->connected = false;
            goto insert;
        }

        ws_log(&log_ctx, LOG_DEBUG, "Found a valid connector with %d modes.",
                conn->count_modes);

        //!< @todo: Do not just take the biggest mode available
        int j = conn->count_modes;
        while (j--) {

            struct ws_monitor_mode* mode =
                ws_monitor_copy_mode(new_monitor, &conn->modes[j]);

            ws_set_insert(&new_monitor->modes, (struct ws_object*)  mode);

            // ws_monitor_add_mode(new_monitor, conn->modes[j].hdisplay,
            //                        conn->modes[j].vdisplay);
        }


        if (find_crtc(res, conn, new_monitor) < 0) {
            ws_log(&log_ctx, LOG_DEBUG, "No valid crtcs found");
            new_monitor->connected = false;
            goto insert;
        }
        ws_log(&log_ctx, LOG_DEBUG,
                "Found a valid crtc with id %d", new_monitor->crtc);
        new_monitor->connected = true;

insert:
        new_monitor->id = i;
        ws_set_insert(&ws_comp_ctx.monitors, (struct ws_object*)new_monitor);
    }
    return 0;
}

static int
populate_framebuffers(
    void* dummy,
    void const* mon
) {
    struct ws_monitor* monitor = (struct ws_monitor*) mon;
    ws_monitor_populate_fb(monitor);
    return 0;
}
