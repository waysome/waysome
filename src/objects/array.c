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
#include <stdlib.h>

#include "objects/array.h"

#include "objects/object.h"

/*
 *
 *
 * Forward declarations
 *
 *
 */

/**
 * Init callback for array type
 *
 * @return true if initialisation worked, else false
 */
static bool
init_callback(
    struct ws_object* self //!< Array object
);

/**
 * Deinit callback for array type
 */
static bool
deinit_callback(
    struct ws_object* const //!< Object to deinitialize
);

/*
 *
 *
 * v-table
 *
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_ARRAY = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_array",

    .init_callback = init_callback,
    .deinit_callback = deinit_callback,
};

/*
 *
 *
 * Interface implementation
 *
 *
 */

struct ws_array*
ws_array_new(void)
{
    struct ws_array* a = calloc(1, sizeof(*a));

    if (a) {
        ws_object_init(&a->obj);
    }

    return a;
}

size_t
ws_array_get_len(
    struct ws_array* const self
) {
    if (self) {
        size_t len;
        ws_object_lock_read(&self->obj);
        len = self->len;
        ws_object_unlock(&self->obj);
        return len;
    }

    return 0;
}

void
ws_array_sort(
    struct ws_array* self,
    signed int (*cmp)(const void*, const void*)
) {
    if (self) {
        ws_object_lock_write(&self->obj);
        qsort(self->ary, sizeof(*self->ary), self->len, cmp);
        ws_object_unlock(&self->obj);
    }
    return;
}

bool
ws_array_has(
    struct ws_array* const self,
    void* const obj
) {
    bool res = false;

    if (self && obj) {
        ws_object_lock_read(&self->obj);
        size_t i;
        for (i = 0; i < self->len && !res; i++) {
            res = (self->ary[i] == obj);
        }

        ws_object_unlock(&self->obj);
    }
    return res;
}

void*
ws_array_find(
    struct ws_array* const self,
    bool (*cmp)(void* const)
) {
    void* res = NULL;

    if (self) {
        ws_object_lock_read(&self->obj);

        size_t i;
        for (i = 0; i < self->len && res == NULL; i++) {
            if (cmp(self->ary[i])) {
                res = self->ary[i];
            }
        }

        ws_object_unlock(&self->obj);
    }

    return res;
}

void*
ws_array_get_at(
    struct ws_array* const self,
    unsigned int i
) {
    void* res = NULL;

    if (self) {
        ws_object_lock_read(&self->obj);

        if (self->len > i) {
            res = self->ary[i];
        }

        ws_object_unlock(&self->obj);
    }

    return res;
}

void*
ws_array_set_at(
    struct ws_array* self,
    void* obj,
    unsigned int i
) {
    void* res = NULL;

    if (self) {
        ws_object_lock_write(&self->obj);

        if (self->len > i) {
            if (self->ary[i]) {
                res = self->ary[i];
            } else {
                res = obj;
            }
            self->ary[i] = obj;
        }

        ws_object_unlock(&self->obj);
    }

    return res;
}

bool
ws_array_foreach(
    struct ws_array* const self,
    bool (*iter)(void* etc, void* entry),
    void* etc
) {
    bool res = false;

    if (self) {
        ws_object_lock_write(&self->obj);

        size_t i;
        for (i = 0, res = true; i < self->len && res; i++) {
            res = iter(etc, self->ary[i]);
        }

        ws_object_unlock(&self->obj);
    }

    return res;
}

int
ws_array_append(
    struct ws_array* const self,
    void* element
) {
    static const int realloc_fact = 2;
    if (!self) {
        return -EINVAL;
    }

    ws_object_lock_write(&self->obj);

    size_t newsize = sizeof(*self->ary) * self->len * realloc_fact;
    void** newbuf = realloc(self->ary, newsize);

    if (!newbuf) {
        ws_object_unlock(&self->obj);
        return -ENOMEM;
    }

    self->ary = newbuf;
    self->ary[self->len] = element;
    self->len *= realloc_fact;
    self->nused++;

    ws_object_unlock(&self->obj);
    return 0;
}

/*
 *
 *
 * static function implementations
 *
 *
 */

static bool
init_callback(
    struct ws_object* obj
) {
    if (obj->id != &WS_OBJECT_TYPE_ID_ARRAY) {
        return false;
    }

    struct ws_array* self = (struct ws_array*) obj;
    if (self) {
        self->len = 2; // initialize with two elements
        self->ary = calloc(self->len, sizeof(*self->ary));

        if (!self->ary) {
            return false;
        }

        ws_object_init(&self->obj);
        self->obj.id = &WS_OBJECT_TYPE_ID_ARRAY;

        self->nused = 0;
        self->sorted = false;

        return true;
    }

    return false;
}

static bool
deinit_callback(
    struct ws_object* self
) {
    free(((struct ws_array*) self)->ary);
    return true;
}
