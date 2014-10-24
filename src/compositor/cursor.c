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

#include <malloc.h>
#include <string.h>
#include <wayland-server.h>
#include <xf86drmMode.h>

#include "objects/object.h"
#include "util/arithmetical.h"

#include "compositor/buffer/frame.h"
#include "compositor/cursor.h"
#include "compositor/framebuffer_device.h"
#include "compositor/internal_context.h"
#include "compositor/monitor.h"
#include "compositor/wayland/client.h"
#include "compositor/wayland/pointer.h"
#include "compositor/wayland/surface.h"
#include "compositor/wayland/region.h"
#include "util/wayland.h"

#define CURSOR_SIZE 64

/**
 *  Deinit the global cursor
 */
static bool
deinit_cursor(
        struct ws_object* self //!< The cursor to deinit
);


ws_object_type_id WS_OBJECT_TYPE_ID_CURSOR = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_cursor",

    .deinit_callback    = deinit_cursor,
    .dump_callback      = NULL,
    .run_callback       = NULL,
    .hash_callback      = NULL,
};


/**
 *
 * Implementations
 *
 */


struct ws_cursor*
ws_cursor_new(
    struct ws_framebuffer_device* dev,
    struct ws_image_buffer* cur
) {
    struct ws_cursor* self = calloc(1, sizeof(*self));
    ws_object_init((struct ws_object*) self);
    self->obj.id = &WS_OBJECT_TYPE_ID_CURSOR;
    self->cur_fb_dev = dev;
    self->cursor_fb = ws_frame_buffer_new(dev, CURSOR_SIZE, CURSOR_SIZE);
    // We set the cursor to sane values
    self->x_hp = 1;
    self->y_hp = 1;
    self->x = 350;
    self->y = 350;
    self->default_cursor = cur;
    ws_buffer_blit((struct ws_buffer*) self->cursor_fb,
                    (struct ws_buffer*) self->default_cursor);

    return self;
}


static int
get_surface_under_cursor(
    void* target,
    void const* _surface
) {
    struct ws_cursor* self = ws_comp_ctx.cursor;
    int real_x = self->x + self->x_hp;
    int real_y = self->y + self->y_hp;

    struct ws_surface* surface = (struct ws_surface*) _surface;

    //<! @todo: Once a surface has a position, we can really check if the
    // cursor is inside
    int x = 0;
    int y = 0;
    struct ws_buffer* buff = ws_wayland_buffer_get_buffer(&surface->img_buf);
    int w = ws_buffer_width(buff);
    int h = ws_buffer_height(buff);

    if (real_x < x || real_x > (x + w)) {
        return 0;
    }

    if (real_y < y || real_y > (y + h)) {
        return 0;
    }

    if (ws_region_inside(surface->input_region, x - real_x, y - real_y)) {
        *((struct ws_surface**) target) = (struct ws_surface*) _surface;
        return 1;
    }

    return 0;
}

void
ws_cursor_set_position(
    struct ws_cursor* self,
    int x,
    int y
) {
    int w = ws_buffer_width((struct ws_buffer*) self->cur_mon->buffer);
    int h = ws_buffer_height((struct ws_buffer*) self->cur_mon->buffer);

    // We use the negative hotspot position because we don't want it to go
    // off screen.
    self->x = CLAMP(-self->x_hp, x, w);
    self->y = CLAMP(-self->y_hp, y, h);
    int retval = drmModeMoveCursor(self->cur_fb_dev->fd, self->cur_mon->crtc,
                                    self->x, self->y);
    if (retval != 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not move cursor");
    }

    struct ws_set* surfaces = ws_monitor_surfaces(self->cur_mon);
    struct ws_surface* nxt_surface = NULL;
    ws_set_select(surfaces, NULL, NULL, get_surface_under_cursor, &nxt_surface);

    if (self->active_surface == nxt_surface) {
        return;
    }
    struct ws_surface* old_surface = self->active_surface;
    self->active_surface = nxt_surface;


    struct wl_display* display = ws_wayland_acquire_display();
    if (!display) {
        return;
    }

    // Did we leave the old surface? Well, send a leave event
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) old_surface);
    if (old_surface != self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* cursor;
        wl_list_for_each(cursor, &client->resources, link) {
            int retval = ws_wayland_pointer_instance_of(cursor->resource);
            if (!retval) {
                continue;
            }
            uint32_t serial = wl_display_next_serial(display);
            wl_pointer_send_leave(cursor->resource, serial, res);
        }

        ws_cursor_set_image(self, NULL);
        ws_log(&log_ctx, LOG_DEBUG, "Left surface!");
    }

    // Did we enter a new surface? Send a enter event!
    res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) self->active_surface);
    if (self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* cursor = NULL;
        //<! @todo once windows have a real position, set it here!
        int surface_x = 0;
        int surface_y = 0;
        wl_list_for_each(cursor, &client->resources, link) {
            int retval = ws_wayland_pointer_instance_of(cursor->resource);
            if (!retval) {
                continue;
            }
            uint32_t serial = wl_display_next_serial(display);
            wl_pointer_send_enter(cursor->resource, serial, res,
                    self->x - surface_x,
                    self->y - surface_y);
        }
        ws_log(&log_ctx, LOG_DEBUG, "Entered surface!");
    }

    ws_wayland_release_display();

}

void
ws_cursor_add_position(
    struct ws_cursor* self,
    int x,
    int y
) {
    ws_cursor_set_position(self, self->x + x, self->y + y);
}

void
ws_cursor_set_hotspot(
    struct ws_cursor* self,
    int x,
    int y
) {
    self->x_hp = CLAMP(0, x, CURSOR_SIZE);
    self->y_hp = CLAMP(0, y, CURSOR_SIZE);
}

void
ws_cursor_redraw(
    struct ws_cursor* self
) {
    int w = ws_buffer_width((struct ws_buffer*) self->cursor_fb);
    int h = ws_buffer_height((struct ws_buffer*) self->cursor_fb);

    int retval = drmModeSetCursor2(self->cur_fb_dev->fd, self->cur_mon->crtc,
            self->cursor_fb->handle, w, h, self->x_hp, self->y_hp);
    if (retval != 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not set cursor");
        ws_log(&log_ctx, LOG_CRIT, "State was: crtc: %d, handle: %d, "
                "height: %d, width: %d",
                self->cur_mon->crtc, self->cursor_fb->handle, w, h);
    }
    retval = drmModeMoveCursor(self->cur_fb_dev->fd, self->cur_mon->crtc,
            self->x, self->y);
    if ( retval != 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not move cursor");
    }
}

void
ws_cursor_set_image(
    struct ws_cursor* self,
    struct ws_buffer* img
) {
    memset(self->cursor_fb->obj.buffer, 0,
            ws_buffer_stride((struct ws_buffer*) self->cursor_fb) *
            ws_buffer_height((struct ws_buffer*) self->cursor_fb));
    ws_log(&log_ctx, LOG_DEBUG, "Setting new cursor image");
    ws_buffer_blit((struct ws_buffer*) self->cursor_fb, img);
}

void
ws_cursor_set_monitor(
    struct ws_cursor* self,
    struct ws_monitor* mon
) {
    self->cur_mon = mon;
}

void
ws_cursor_unset(
    struct ws_cursor* self
) {
    //<! @todo: Make unsetting work
    int retval = drmModeSetCursor(self->cur_fb_dev->fd, self->cur_mon->crtc,
            self->cursor_fb->handle, 0, 0);
    ws_log(&log_ctx, LOG_DEBUG, "Removing cursor: %d", retval);
}

struct ws_cursor*
ws_cursor_get()
{
    return ws_comp_ctx.cursor;
}

static bool
deinit_cursor(
        struct ws_object* s
) {
    struct ws_cursor* self = (struct ws_cursor*) s;
    ws_object_deinit((struct ws_object*) self->cursor_fb);
    return true;
}

