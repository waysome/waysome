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

#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>

#include "objects/object.h"

/*
 * Type information
 */
ws_object_id WS_OBJECT_ID_OBJECT = {
    .supertype  = &WS_OBJECT_ID_OBJECT,
    .typestr    = "ws_object",
};

struct ws_object*
ws_object_new(
    size_t s
) {
    if (s < sizeof(struct ws_object)) {
        return NULL;
    }

    struct ws_object* o = calloc(1, s);

    if (o) {
        o->id = &WS_OBJECT_ID_OBJECT;
        o->settings = WS_OBJ_NO_SETTINGS;
        atomic_store(&o->refcnt, 1);

        pthread_mutex_init(&o->read_lock, NULL);
        pthread_mutex_init(&o->write_lock, NULL);
    }

    return o;
}

struct ws_object*
ws_object_new_raw(void) {
    return ws_object_new(sizeof(struct ws_object));
}

ws_object_id*
ws_object_get_id(
    struct ws_object const* const self
) {
    if (self) {
        return self->id;
    }
    return NULL;
}

enum ws_object_settings
ws_object_get_settings(
    struct ws_object const* const self
) {
    if (self) {
        return self->settings;
    }

    return WS_OBJ_NO_SETTINGS;
}

void
ws_object_set_settings(
    struct ws_object* self,
    enum ws_object_settings settings
) {
    if (self) {
        self->settings = settings;
    }
}

bool
ws_object_init(
    struct ws_object* self
) {
    if (self) {
        atomic_store(&self->refcnt, 1);
        self->settings = WS_OBJ_NO_SETTINGS;

        pthread_mutex_init(&self->read_lock, NULL);
        pthread_mutex_init(&self->write_lock, NULL);

        if (self->id) {
            self->id->init_callback(self);
        } else {
            self->id = &WS_OBJECT_ID_OBJECT;
        }

        return true;
    }

    return false;
}

struct ws_object*
ws_object_getref(
    struct ws_object* self
) {
    if (self) {
        atomic_fetch_add(&self->refcnt, 1);
        return self;
    }

    return NULL;
}

void
ws_object_unref(
    struct ws_object* self
) {
    /** @todo implement */
    return;
}

bool
ws_object_log(
    struct ws_object const* const self,
    void* log_context /*!< @todo ws_log_context* */
) {
    /** @todo implement */
    return false;
}

bool
ws_object_run(
    struct ws_object* self
) {
    if (self && self->id && self->id->run_callback) {
        return self->id->run_callback(self);
    }

    return false;
}

bool
ws_object_lock_read(
    struct ws_object* self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_lock_write(
    struct ws_object* self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_unlock_read(
    struct ws_object* self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_unlock_write(
    struct ws_object* self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_is_locked(
    struct ws_object const* const self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_is_read_locked(
    struct ws_object const* const self
) {
    /** @todo implement */
    return false;
}

bool
ws_object_is_write_locked(
    struct ws_object const* const self
) {
    /** @todo implement */
    return false;
}
