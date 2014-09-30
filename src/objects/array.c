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

    .deinit_callback = deinit_callback,
};

/*
 *
 *
 * Interface implementation
 *
 *
 */

bool
ws_array_init(
    struct ws_array* self
) {
    /** @todo implement */
    return false;
}

struct ws_array*
ws_array_new(void)
{
    /** @todo implement */
    return NULL;
}

size_t
ws_array_get_len(
    struct ws_array* const self
) {
    /** @todo implement */
    return 0;
}

void
ws_array_sort(
    struct ws_array* self,
    signed int (*cmp)(const void*, const void*)
) {
    /** @todo implement */
    return;
}

bool
ws_array_has(
    struct ws_array* const self,
    void* const obj
) {
    /** @todo implement */
    return false;
}

void*
ws_array_find(
    struct ws_array* const self,
    bool (*cmp)(void* const)
) {
    /** @todo implement */
    return NULL;
}

void*
ws_array_get_at(
    struct ws_array* const self,
    unsigned int i
) {
    /** @todo implement */
    return NULL;
}

void*
ws_array_set_at(
    struct ws_array* self,
    void* obj,
    unsigned int i
) {
    /** @todo implement */
    return NULL;
}

bool
ws_array_foreach(
    struct ws_array* const self,
    bool (*iter)(void* etc, void* entry),
    void* etc
) {
    /** @todo implement */
    return false;
}

int
ws_array_append(
    struct ws_array* const self,
    void* element
) {
    /** @todo implement */
    return false;
}

/*
 *
 *
 * static function implementations
 *
 *
 */

static bool
deinit_callback(
    struct ws_object* self
) {
    free(((struct ws_array*) self)->ary);
    return true;
}
