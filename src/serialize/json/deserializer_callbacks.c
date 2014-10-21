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

#include "serialize/deserializer.h"
#include "serialize/json/deserializer_state.h"
#include "serialize/json/deserializer_callbacks.h"
#include "serialize/json/states.h"
#include "serialize/json/string_jump_state.h"
#include "values/nil.h"
#include "values/bool.h"

int
yajl_null_cb(
    void * ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            struct ws_value_nil* nil = calloc(1, sizeof(*nil));
            if (!nil) {
                //!< @todo error
                return 0;
            }
            ws_value_nil_init(nil);

            if (0 != ws_statement_append_direct(state->tmp_statement,
                                                (struct ws_value*) nil)) {
                //!< @todo error
                return 0;
            }

            state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        }
        break;

    default:
        state->current_state = STATE_INVALID;
        break;

    }
    return 1;
}

int
yajl_boolean_cb(
    void * ctx,
    int b
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            struct ws_value_bool* boo = calloc(1, sizeof(*boo));
            if (!boo) {
                //!< @todo error
                return 0;
            }

            ws_value_bool_init(boo);
            ws_value_bool_set(boo, b);
            if (0 != ws_statement_append_direct(state->tmp_statement,
                                                (struct ws_value*) boo)) {
                //!< @todo error
                return 0;
            }
            state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        }
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_integer_cb(
    void * ctx,
    long long i
) {
    //!< @todo implement
    return 0;
}

int
yajl_double_cb(
    void * ctx,
    double d
) {
    //!< @todo implement
    return 0;
}

int
yajl_string_cb(
    void * ctx,
    const unsigned char * str,
    size_t len
) {
    //!< @todo implement
    return 0;
}

int
yajl_start_map_cb(
    void * ctx
) {
    //!< @todo implement
    return 0;
}

int
yajl_map_key_cb(
    void * ctx,
    const unsigned char * key,
    size_t len
) {
    //!< @todo implement
    return 0;
}

int
yajl_end_map_cb(
    void * ctx
) {
    //!< @todo implement
    return 0;
}

int
yajl_start_array_cb(
    void * ctx
) {
    //!< @todo implement
    return 0;
}

int
yajl_end_array_cb(
    void * ctx
) {
    //!< @todo implement
    return 0;
}

