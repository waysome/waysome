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
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <wayland-util.h>
#include <wayland-server.h>
#include <wayland-server-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include "objects/object.h"
#include "objects/wayland_obj.h"
#include "compositor/keyboard.h"
#include "compositor/internal_context.h"
#include "compositor/wayland/client.h"
#include "compositor/wayland/keyboard.h"
#include "util/wayland.h"

static const char keymap_file_template[] = "waysome-xkb-keymap-XXXXXX";

struct xkb {
    struct xkb_context * context;
    struct xkb_state * state;

    struct
    {
        struct xkb_keymap * map;
        int fd;
        uint32_t size;
        char * area;
    } keymap;

    struct
    {
        uint32_t ctrl;
        uint32_t alt;
        uint32_t super;
        uint32_t shift;
    } indices;
};

/**
 *
 */
static int
xkb_init(
    struct ws_keyboard* self //!< keyboard
);

/**
 * Deinit callback for ws_keyboard type
 */
bool
deinit_keyboard(
    struct ws_object* self //!< The object
);

/**
 *
 */
static int
update_keymap(
    struct xkb* xkb //!< XKB object
);

ws_object_type_id WS_OBJECT_TYPE_ID_KEYBOARD = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_keyboard",

    .deinit_callback    = deinit_keyboard,
    .hash_callback      = NULL,
};

/*
 *
 * Interface implementation
 *
 */

struct ws_keyboard*
ws_keyboard_new(void)
{
    struct ws_keyboard* k = calloc(1, sizeof(*k));

    if (!k) {
        return NULL;
    }

    ws_object_init(&k->obj);

    k->obj.id = &WS_OBJECT_TYPE_ID_KEYBOARD;
    k->obj.settings |= WS_OBJECT_HEAPALLOCED;
    k->active_surface = NULL;

    wl_array_init(&k->pressed_keys);

    if (0 != xkb_init(k)) {
        ws_object_unref(&k->obj);
        return NULL;
    }

    return k;
}

struct ws_keyboard*
ws_keyboard_get(void)
{
    return ws_comp_ctx.keyboard;
}

void
ws_keyboard_send_keymap(
    struct ws_keyboard* self
) {
    // Did we leave the old surface? Well, send a leave event
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) self->active_surface);

    if (self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* keyboard;
        wl_list_for_each(keyboard, &client->resources, link) {
            int retval = ws_wayland_keyboard_instance_of(keyboard->resource);
            if (!retval) {
                continue;
            }
            wl_keyboard_send_keymap(keyboard->resource,
                                    WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,
                                    self->xkb->keymap.fd,
                                    self->xkb->keymap.size - 1);
        }
    }
}

void
ws_keyboard_send_enter(
    struct ws_keyboard* self
) {
    struct wl_display* d = ws_wayland_acquire_display();

    if (!d) {
        return;
    }

    // Did we leave the old surface? Well, send a leave event
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) self->active_surface);

    if (self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* keyboard;
        wl_list_for_each(keyboard, &client->resources, link) {
            int retval = ws_wayland_keyboard_instance_of(keyboard->resource);
            if (!retval) {
                continue;
            }
            uint32_t serial = wl_display_next_serial(d);
            wl_keyboard_send_enter(keyboard->resource, serial, res,
                                   &self->pressed_keys);
        }
    }

    ws_wayland_release_display();
}

void
ws_keyboard_send_leave(
    struct ws_keyboard* self
) {
    struct wl_display* d = ws_wayland_acquire_display();

    if (!d) {
        return;
    }

    // Did we leave the old surface? Well, send a leave event
    struct wl_resource* res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) self->active_surface);

    if (self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* keyboard;
        wl_list_for_each(keyboard, &client->resources, link) {
            int retval = ws_wayland_keyboard_instance_of(keyboard->resource);
            if (!retval) {
                continue;
            }
            uint32_t serial = wl_display_next_serial(d);
            wl_keyboard_send_leave(keyboard->resource, serial, res);
        }
    }

    ws_wayland_release_display();
}

void
ws_keyboard_send_key(
    struct ws_keyboard* self,
    struct timeval* time,
    uint32_t key,
    uint32_t state
) {
    struct wl_display* d = ws_wayland_acquire_display();

    if (!d) {
        return;
    }

    enum xkb_key_direction direction;
    direction = (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? XKB_KEY_DOWN
                                                         : XKB_KEY_UP;
    int ret = xkb_state_update_key(self->xkb->state, key + 8, direction);

    // If the next press does /nothing/ to the state we just remove the press,
    // this way we do not get stuck modifiers, this might be hacky
    if (direction == XKB_KEY_DOWN && !ret) {
        xkb_state_update_key(self->xkb->state, key + 8, XKB_KEY_UP);
    }

    uint32_t depressed;
    uint32_t group;
    uint32_t locked;
    uint32_t latched;

    depressed = xkb_state_serialize_mods(self->xkb->state, XKB_STATE_DEPRESSED);
    latched   = xkb_state_serialize_mods(self->xkb->state, XKB_STATE_LATCHED);
    locked    = xkb_state_serialize_mods(self->xkb->state, XKB_STATE_LOCKED);
    group     = xkb_state_serialize_layout(self->xkb->state,
                                           XKB_STATE_LAYOUT_EFFECTIVE);

    struct wl_resource* res = ws_wayland_obj_get_wl_resource(
            (struct ws_wayland_obj*) self->active_surface);

    if (self->active_surface && res) {
        struct ws_wayland_client* client = ws_wayland_client_get(res->client);

        struct ws_deletable_resource* keyboard;
        wl_list_for_each(keyboard, &client->resources, link) {
            int retval = ws_wayland_keyboard_instance_of(keyboard->resource);
            if (!retval) {
                continue;
            }
            uint32_t serial = wl_display_next_serial(d);
            uint32_t t = (time->tv_sec * 1000) + (time->tv_usec / 1000);
            wl_keyboard_send_modifiers(keyboard->resource, serial, depressed,
                                        latched, locked, group);
            wl_keyboard_send_key(keyboard->resource, serial, t, key, state);
        }
    }

    ws_wayland_release_display();
}

void
ws_keyboard_send_modifiers(
    struct ws_keyboard* self,
    uint32_t serial,
    uint32_t mods_depressed,
    uint32_t mods_latched,
    uint32_t mods_locked,
    uint32_t group
) {
    //!< @todo
}

/*
 *
 * static function implementations
 *
 */

static int
xkb_init(
    struct ws_keyboard* self //!< keyboard
) {
    self->xkb = calloc(1, sizeof(*self->xkb));

    if (!self->xkb) {
        return -ENOMEM;
    }

    self->xkb->context = xkb_context_new(0);

    if (!self->xkb->context) {
        return -ENOMEM;
    }

    // NULL and zero are defaults here, so they are hardcoded
    self->xkb->keymap.map = xkb_keymap_new_from_names(self->xkb->context,
                                                      NULL, 0);

    if (!self->xkb->keymap.map) {
        xkb_context_unref(self->xkb->context);
        return -ENOENT;
    }

    self->xkb->state = xkb_state_new(self->xkb->keymap.map);

    if (!self->xkb->state) {
        xkb_keymap_unref(self->xkb->keymap.map);
        xkb_context_unref(self->xkb->context);
        return -ENOMEM;
    }

    int ret = update_keymap(self->xkb);
    if (ret != 0) {
        xkb_state_unref(self->xkb->state);
        xkb_keymap_unref(self->xkb->keymap.map);
        xkb_context_unref(self->xkb->context);
        return ret;
    }

    return 0;
}

static int
update_keymap(
    struct xkb* xkb
) {
    const char* tmp_km_dir = getenv("XDG_RUNTIME_DIR");
    const char * keymap_dir = tmp_km_dir ? tmp_km_dir : "/tmp";
    char * keymap_string;
    size_t km_size = strlen(keymap_dir) + sizeof(keymap_file_template) + 1;
    char keymap_path[km_size];

    xkb->indices.ctrl
        = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_CTRL);
    xkb->indices.alt
        = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_ALT);
    xkb->indices.super
        = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_LOGO);
    xkb->indices.shift
        = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_SHIFT);

    /* In order to send the keymap to clients, we must first convert it to a
     * string and then mmap it to a file. */
    keymap_string = xkb_keymap_get_as_string(xkb->keymap.map,
            XKB_KEYMAP_FORMAT_TEXT_V1);

    if (!keymap_string) {
        // Could not get XKB keymap as a string
        goto error0;
    }

    sprintf(keymap_path, "%s/%s", keymap_dir, keymap_file_template);

    xkb->keymap.size    = strlen(keymap_string) + 1;
    xkb->keymap.fd      = mkstemp(keymap_path);
    if (xkb->keymap.fd == -1) {
        // Could not create XKB keymap file
        goto error1;
    }

    if (fcntl(xkb->keymap.fd, F_SETFD, FD_CLOEXEC) < 0) {
        goto error2;
    }

    unlink(keymap_path);

    if (posix_fallocate(xkb->keymap.fd, 0, xkb->keymap.size) != 0) {
        // Could not resize XKB keymap file
        goto error2;
    }

    xkb->keymap.area = mmap(NULL, xkb->keymap.size, PROT_READ | PROT_WRITE,
                            MAP_SHARED, xkb->keymap.fd, 0);

    if (xkb->keymap.area == MAP_FAILED) {
        // Could not mmap XKB keymap string
        goto error2;
    }

    strcpy(xkb->keymap.area, keymap_string);
    free(keymap_string);
    return 0;

error2:
    close(xkb->keymap.fd);

error1:
    free(keymap_string);

error0:
    return -1;
}

bool
deinit_keyboard(
    struct ws_object* self
) {
    struct ws_keyboard* kb = (struct ws_keyboard*) self;

    xkb_state_unref(kb->xkb->state);
    kb->xkb->state = NULL;

    xkb_keymap_unref(kb->xkb->keymap.map);
    kb->xkb->keymap.map = NULL;

    xkb_context_unref(kb->xkb->context);
    kb->xkb->context = NULL;

    free(kb->xkb);
    kb->xkb = NULL;

    return true;
}

