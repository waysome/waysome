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

#include "compositor/monitor_mode.h"

/*
 *
 *  Forward declarations
 *
 */

static size_t
monitor_mode_hash(
    struct ws_object* self
);

static int
monitor_mode_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/*
 *
 * Type variable
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_MONITOR_MODE = {
    .supertype = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr = "ws_monitor_mode",
    .deinit_callback = NULL,
    .hash_callback = monitor_mode_hash,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = monitor_mode_cmp
};

/*
 *
 * Interface implementation
 *
 */

struct ws_monitor_mode*
ws_monitor_mode_new(
    void
) {
    struct ws_monitor_mode* tmp = calloc(1, sizeof(*tmp));
    ws_object_init((struct ws_object*) tmp);
    tmp->obj.id = &WS_OBJECT_TYPE_ID_MONITOR_MODE;
    tmp->obj.settings |= WS_OBJECT_HEAPALLOCED;

    return tmp;
}

/*
 *
 * static function implementations
 *
 */

static size_t
monitor_mode_hash(
    struct ws_object* obj
) {
    struct ws_monitor_mode* self = (struct ws_monitor_mode*) obj;
    return SIZE_MAX / (self->id + 1);
}

static int
monitor_mode_cmp(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_monitor_mode* mon1 = (struct ws_monitor_mode*) obj1;
    struct ws_monitor_mode* mon2 = (struct ws_monitor_mode*) obj2;

    if (mon1->id != mon2->id) {
        return (mon1->id > mon2->id) - (mon1->id < mon2->id);
    }
    return 0;
}
