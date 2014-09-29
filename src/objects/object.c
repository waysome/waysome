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

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include "objects/object.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Alias: `pthread_rwlock_rdlock(&obj->rw_lock);
 */
static inline void
rdlock(
    struct ws_object* const obj //!< The object to read-lock
);

/**
 * Alias: `pthread_rwlock_wrlock(&obj->rw_lock);
 */
static inline void
wrlock(
    struct ws_object* const obj //!< The object to write-lock
);

/**
 * Alias: `pthread_rwlock_unlock(&obj->rw_lock);
 */
static inline void
unlock(
    struct ws_object* const obj //!< The object to unlock
);


/*
 * Type information
 */
ws_object_type_id WS_OBJECT_TYPE_ID_OBJECT = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
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
        o->id = &WS_OBJECT_TYPE_ID_OBJECT;
        o->settings = WS_OBJ_NO_SETTINGS;
        pthread_rwlock_init(&o->rw_lock, NULL);
    }

    return o;
}

struct ws_object*
ws_object_new_raw(void) {
    return ws_object_new(sizeof(struct ws_object));
}

ws_object_type_id*
ws_object_get_id(
    struct ws_object* const self
) {
    if (self) {
        ws_object_type_id* id;
        rdlock(self);
        id = self->id;
        unlock(self);
        return id;
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
        self->settings = WS_OBJ_NO_SETTINGS;

        pthread_rwlock_init(&self->rw_lock, NULL);

        if (self->id) {
            self->id->init_callback(self);
        } else {
            self->id = &WS_OBJECT_TYPE_ID_OBJECT;
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
        wrlock(self);
        self->ref_counting.refcnt++;
        unlock(self);
        return self;
    }

    return NULL;
}

void
ws_object_unref(
    struct ws_object* self
) {
    if (self) {
        wrlock(self);
        self->ref_counting.refcnt--;
        self->id->unref_callback(self);

        if (self->ref_counting.refcnt == 0) {
            self->id->free_callback(self);
            free(self);
        } else {
            unlock(self);
        }
    }
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
    return 0 == pthread_rwlock_rdlock(&self->rw_lock);
}

bool
ws_object_lock_write(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_wrlock(&self->rw_lock);
}

bool
ws_object_unlock_read(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_unlock(&self->rw_lock);
}

bool
ws_object_unlock_write(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_unlock(&self->rw_lock);
}

/*
 *
 * static function implementations
 *
 */

static inline void
rdlock(
    struct ws_object* const obj
) {
    pthread_rwlock_rdlock(&obj->rw_lock);
}

static inline void
wrlock(
    struct ws_object* const obj //!< The object to write-lock
) {
    pthread_rwlock_wrlock(&obj->rw_lock);
}

static inline void
unlock(
    struct ws_object* const obj
) {
    pthread_rwlock_unlock(&obj->rw_lock);
}
