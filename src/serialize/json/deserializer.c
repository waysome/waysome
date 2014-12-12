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
 * @file json/deserializer.c
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
deserialize_state_new(
    yajl_callbacks* cbs,
    void* ctx
);

/**
 * Initialize the passed deserializer_state object
 */
static void
deserialize_state_init(
    struct deserializer_state* self
);

/**
 * Initialize yajl handle
 *
 * @return zero on success
 */
static int
initialize_yajl(
    struct deserializer_state* self, //!< The deserializer state object
    yajl_callbacks* cbs, //!< The callback table to use
    void* ctx //!< The context
);

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

static yajl_callbacks YAJL_CALLBACKS = {
    .yajl_null          = yajl_null_cb,
    .yajl_boolean       = yajl_boolean_cb,
    .yajl_integer       = yajl_integer_cb,
    .yajl_double        = NULL, // no double support
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

    ws_log(&log_ctx, LOG_DEBUG, "Allocated deserializer");
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

    deserialize_state_init(state);

    if (initialize_yajl(state, cbs, ctx)) {
        yajl_free(state->handle);
        free(state);
        return NULL;
    }

    return state;
}

static void
deserialize_state_init(
    struct deserializer_state* self
) {
    memset(self, 0, sizeof(*self));
    self->current_state = STATE_INIT;
    ws_log(&log_ctx, LOG_DEBUG, "Allocated deserializer internal state");
}

static int
initialize_yajl(
    struct deserializer_state* self,
    yajl_callbacks* cbs,
    void* ctx
) {
    self->handle = yajl_alloc(cbs, NULL, ctx);

    if (!yajl_config(self->handle, yajl_allow_comments, 1)) {
        return 1;
    }
    if (!yajl_config(self->handle, yajl_allow_trailing_garbage, 1)) {
        return 1;
    }
    if (!yajl_config(self->handle, yajl_allow_multiple_values, 1)) {
        return 1;
    }

    return 0;
}

static ssize_t
deserialize(
    struct ws_deserializer* self,
    char const* buf,
    size_t nbuf
) {
    unsigned char* buffer           = (unsigned char*) buf;
    struct deserializer_state* d    = self->state;
    size_t consumed                 = 0;

    ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: Start", self);

    while (isspace(*buffer)) {
        buffer++;
        consumed++;
        if (--nbuf == 0) {
            self->is_ready = true;
            ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: Stop", self);
            return consumed;
        }
    }

    ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: Start parsing", self);
    yajl_status stat = yajl_parse(d->handle, buffer, nbuf);
    ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: YAJL-State: %s",
           self, yajl_status_to_string(stat));
    ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: Parsing finished", self);

    if (stat == yajl_status_client_canceled && !self->is_ready) {
        // the parsing resulted in an error.
        static char* y_err_fmt = "YAJL parser error (code %i)";
        static char* d_err_fmt = "Deserializer error (code %i)";
        char* fmt;
        if (d->error.parser_error) {
            fmt = y_err_fmt;
        } else {
            fmt = d_err_fmt;
        }

        ws_log(&log_ctx, LOG_ERR, fmt, d->error.error_num);
    }

    if ((d->current_state == STATE_INVALID) && self->buffer) {
        ws_object_deinit(&self->buffer->obj);
        self->buffer = NULL;
    }

    if (yajl_status_error == stat) {
        unsigned char* errstr = yajl_get_error(d->handle, 1, buffer, nbuf);
        ws_log(&log_ctx, LOG_ERR,
               "We have an error in the JSON deserializing: %s", errstr);
        yajl_free_error(d->handle, errstr);
    }

    ws_log(&log_ctx, LOG_DEBUG, "[Deserializer %p]: Stop", self);

    consumed += yajl_get_bytes_consumed(d->handle);

    if (stat == yajl_status_client_canceled && d->current_state == STATE_INIT &&
            self->is_ready) {
        ws_log(&log_ctx, LOG_DEBUG,
               "[Deserializer %p]: Ready with a JSON object", self);

        yajl_free(d->handle);

        // first reinit the state object (memsets to zero)
        ws_log(&log_ctx, LOG_DEBUG,
               "[Deserializer %p]: Reinitializing internal state", self);
        deserialize_state_init(d);

        // Now reinitialize yajl
        ws_log(&log_ctx, LOG_DEBUG,
               "[Deserializer %p]: Reinitializing YAJL", self);
        if (initialize_yajl(d, &YAJL_CALLBACKS, self)) {
            ws_log(&log_ctx, LOG_WARNING,
                   "[Deserializer %p]: Reinitializing of YAJL failed", self);
        }
    }

    return consumed;
}

