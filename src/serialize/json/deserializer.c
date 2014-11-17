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

/**
 * @file deserializer.c
 *
 * This file contains the deserializer backend for JSON. For a full
 * documentation of the statemachine @ref json_statemachine "see here".
 *
 * The implementation makes use of the yajl library. For a documentation of the
 * yajl library, see: https://lloyd.github.io/yajl/
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "objects/message/message.h"
#include "serialize/deserializer.h"
#include "serialize/json/deserializer.h"
#include "serialize/json/deserializer_callbacks.h"

#include "logger/module.h"

static struct ws_logger_context log_ctx = {
    .prefix = "[JSON Deserializer] ",
};

/*
 *
 * static function declarations
 *
 */

/**
 * Allocate a new desertializer state object
 *
 * @return new object of `struct serializer_yajl_state_deserializer` or NULL
 */
struct deserializer_state*
deserialize_state_new(yajl_callbacks* cbs, void* ctx);

/**
 * deserialize callback
 *
 * @return negative error value on error, else number of deserialized bytes
 */
static ssize_t
deserialize(
    struct ws_deserializer* self,
    char const* buf,
    size_t nbuf
);

/*
 *
 * variables
 *
 */

//!< @todo implement yajl callbacks

static yajl_callbacks YAJL_CALLBACKS = {
    .yajl_null          = yajl_null_cb,
    .yajl_boolean       = yajl_boolean_cb,
    .yajl_integer       = yajl_integer_cb,
    .yajl_double        = yajl_double_cb,
    .yajl_number        = NULL, // We do not set the number callback
    .yajl_string        = yajl_string_cb,
    .yajl_start_map     = yajl_start_map_cb,
    .yajl_map_key       = yajl_map_key_cb,
    .yajl_end_map       = yajl_end_map_cb,
    .yajl_start_array   = yajl_start_array_cb,
    .yajl_end_array     = yajl_end_array_cb,
};

/*
 *
 * interface implementation
 *
 */

struct ws_deserializer*
ws_serializer_json_deserializer_new(void)
{
    struct ws_deserializer* d = calloc(1, sizeof(*d));
    if (!d) {
        return NULL;
    }

    d->state = (void*) deserialize_state_new(&YAJL_CALLBACKS, d);

    if (!d->state) {
        free(d);
        return NULL;
    }

    d->deserialize = deserialize;

    return d;
}

/*
 *
 * static function implementations
 *
 */

struct deserializer_state*
deserialize_state_new(
    yajl_callbacks* cbs,
    void* ctx
) {
    struct deserializer_state* state = calloc(1, sizeof(*state));
    if (!state) {
        return NULL;
    }

    state->handle = yajl_alloc(cbs, NULL, ctx);

    if (!yajl_config(state->handle, yajl_allow_trailing_garbage, 1) ||
        !yajl_config(state->handle, yajl_allow_multiple_values, 1) ||
        !yajl_config(state->handle, yajl_allow_partial_values, 1)) {

            yajl_free(state->handle);
            return NULL;
    }

    state->current_state    = STATE_INIT;
    state->tmp_statement    = NULL;

    state->nboxbrackets     = 0;
    state->ncurvedbrackets  = 0;

    state->id               = 0;

    // Explicit set to EXEC here.
    state->flags            = WS_TRANSACTION_FLAGS_EXEC;

    state->ev_ctx           = NULL;
    state->ev_name          = NULL;
    state->has_event        = false;

    return state;
}

static ssize_t
deserialize(
    struct ws_deserializer* self,
    char const* buf,
    size_t nbuf
) {
    unsigned char* buffer = (unsigned char*) buf; // cast
    struct deserializer_state* d = self->state;
    size_t consumed = 0;

    while (isspace(*buffer)) {
        buffer++;
        consumed++;
        --nbuf;
        if (nbuf == 0) {
            self->is_ready = true;
            return consumed;
        }
    }

    yajl_status stat = yajl_parse(d->handle, buffer, nbuf);

    if ((d->current_state == STATE_INVALID) && self->buffer) {
        ws_object_deinit(&self->buffer->obj);
        self->buffer = NULL;
    }

    if (yajl_status_error == stat) {
        ws_log(&log_ctx, LOG_ERR, "We have an error in the JSON deserializing");
        unsigned char* errstr;
        errstr = yajl_get_error(d->handle, 1, buffer, nbuf);
        yajl_free_error(d->handle, errstr);
    }

    return yajl_get_bytes_consumed(d->handle) + consumed;
}

