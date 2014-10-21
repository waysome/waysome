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

#include <string.h>

#include "serialize/deserializer.h"
#include "serialize/json/deserializer_state.h"
#include "serialize/json/deserializer_callbacks.h"
#include "serialize/json/keys.h"
#include "serialize/json/states.h"
#include "serialize/json/string_jump_state.h"
#include "values/nil.h"
#include "values/bool.h"
#include "values/int.h"
#include "values/string.h"
#include "wayland-util.h"

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
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_UID:
        if (d->buffer) {
            // Hey, we have a message object, set the ID directly
            d->buffer->id = i;
        } else {
            // cache the ID
            state->id = i; //!< @todo visibility violation here
        }
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            struct ws_value_int* _i = calloc(1, sizeof(_i));
            if (!_i) {
                //!< @todo error
                return 0;
            }

            ws_value_int_init(_i);
            ws_value_int_set(_i, i);
            if (0 != ws_statement_append_direct(state->tmp_statement,
                                                (struct ws_value*) _i)) {
                //!< @todo error
                return 0;
            }

            state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        }
        break;

    case STATE_COMMAND_ARY_COMMAND_ARG_DIRECT_STACKPOS:
        ws_statement_append_indirect(state->tmp_statement, i);
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
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
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_INIT:
        // a strange thing happened
        return 0;

    case STATE_TYPE:
        if (0 == strncmp(TYPE_TRANSACTION, (char*) str,
                    strlen(TYPE_TRANSACTION))) {
            setup_transaction(d);
        }
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            char *buff = strndup((char*) str, len);

            if (!buff) {
                return 0;
            }

            struct ws_value_string* s = ws_value_string_new();
            if (!s) {
                return 0;
            }
            struct ws_string* str = ws_value_string_get(s);
            ws_string_set_from_raw(str, buff);

            free(buff); // ws_string_set_from_raw() copies.

            if (0 != ws_statement_append_direct(state->tmp_statement,
                                                (struct ws_value*) s)) {
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
yajl_start_map_cb(
    void * ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    state->ncurvedbrackets++;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_INIT:
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY:
        // We run into a new command here
        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARG_DIRECT;
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_map_key_cb(
    void * ctx,
    const unsigned char * key,
    size_t len
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_MSG:
        // We're running into a top-level key here, lets decide what comes next:
        state->current_state = get_next_state_for_string(STATE_MSG, key);
        break;

    case STATE_COMMAND_ARY_NEW_COMMAND:
        // This is now the KEY of the command, the command name.
        // The next state should be the command argument array
        {
            char buf[len + 1];
            strncpy(buf, (char*) key, len);
            buf[len] = '\0';

            state->tmp_statement = calloc(1, sizeof(*state->tmp_statement));
            if (!state->tmp_statement) {
                //!< @todo error?
                return 0;
            }

            if (0 != ws_statement_init(state->tmp_statement, buf)) {
                //!< @todo error?
                return 0;
            }

            state->current_state = STATE_COMMAND_ARY_COMMAND_NAME;
        }
        break;

    case STATE_COMMAND_ARY_COMMAND_ARG_DIRECT:
        // If the key is a "pos" key, for a stack position, we continue here

        if (0 != strncmp((char*) key, POS, len)) {
            return 0;
        }
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARG_DIRECT_STACKPOS;
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_end_map_cb(
    void * ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    state->ncurvedbrackets--;

    switch (state->current_state) {
    case STATE_INVALID:
        break;

    case STATE_COMMAND_ARY_NEW_COMMAND:
        // We are ready with the command parsing for one command now. Lets
        // finalize the temporary stuff and go back to the command array state.

        if (state->tmp_statement == NULL) {
            // We are here because there was a command array but no commands.
            // This is absolutely valid.
            state->current_state = STATE_COMMAND_ARY;
            return 1;
        }

        struct ws_transaction* t = (struct ws_transaction*) d->buffer;
        ws_transaction_push_statement(t, state->tmp_statement);
        state->tmp_statement = NULL;

        state->current_state = STATE_COMMAND_ARY;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    case STATE_MSG:
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    if (state->nboxbrackets == 0 && state->ncurvedbrackets == 0) {
        // Hey, we are ready now!
        d->is_ready = true;
        return 0;
    }
    return 1;
}

int
yajl_start_array_cb(
    void * ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    state->nboxbrackets++;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMANDS:
        setup_transaction(d);
        state->current_state = STATE_COMMAND_ARY;
        break;

    case STATE_COMMAND_ARY_COMMAND_NAME:
        // We are in the command name state and the next thing is an array, so
        // we must allocate the command argument array here.
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_end_array_cb(
    void * ctx
) {
    //!< @todo implement
    return 0;
}
