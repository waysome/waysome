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

#include <stdlib.h>
#include <yajl/yajl_parse.h>

#include "serialize/json/common.h"
#include "util/arithmetical.h"

/*
 *
 * static function declarations
 *
 */

static void *
yajl_malloc_func_cb(
    void *ctx,
    size_t sz
);

static void
yajl_free_func_cb(
    void *ctx,
    void * ptr
);

static void *
yajl_realloc_func_cb(
    void *ctx,
    void * ptr,
    size_t sz
);

/*
 *
 * variables
 *
 */

yajl_alloc_funcs YAJL_ALLOC_FUNCS = {
    .malloc     = yajl_malloc_func_cb,
    .realloc    = yajl_realloc_func_cb,
    .free       = yajl_free_func_cb,
    .ctx        = NULL,
};

/*
 *
 * Interface implementation
 *
 */

struct serializer_yajl_state*
serializer_yajl_state_new(
    size_t size,
    yajl_callbacks* callbacks,
    void* ctx
) {
    struct serializer_yajl_state* state = calloc(1, size);
    if (!state) {
        return NULL;
    }

    state->handle = yajl_alloc(callbacks, &YAJL_ALLOC_FUNCS, ctx);

    if (!yajl_config(state->handle, yajl_allow_trailing_garbage, 1) ||
        !yajl_config(state->handle, yajl_allow_multiple_values, 1) ||
        !yajl_config(state->handle, yajl_allow_partial_values, 1)) {

            yajl_free(state->handle);
            return NULL;
    }

    return state;
}

void
serializer_yajl_state_free(
    struct serializer_yajl_state* state
) {
    yajl_free(state->handle);
    free(state);
}

/*
 *
 * static function implementations
 *
 */

static void *
yajl_malloc_func_cb(
    void *ctx,
    size_t sz
) {
    //!< @todo implement
    return NULL;
}

static void
yajl_free_func_cb(
    void *ctx,
    void * ptr
) {
    //!< @todo implement
    return;
}

static void *
yajl_realloc_func_cb(
    void *ctx,
    void * ptr,
    size_t sz
) {
    //!< @todo implement
    return NULL;
}
