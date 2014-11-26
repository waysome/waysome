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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-util.h>
#include <yajl/yajl_parse.h>

#include "command/command.h"
#include "objects/message/event.h"
#include "objects/message/transaction.h"
#include "objects/string.h"
#include "serialize/deserializer.h"
#include "serialize/json/deserializer_callbacks.h"
#include "serialize/json/keys.h"
#include "serialize/json/states.h"
#include "values/bool.h"
#include "values/int.h"
#include "values/nil.h"
#include "values/string.h"
#include "wayland-util.h"

/**
 * Map for mapping the current state with a string on the next state
 */
static const struct {
    enum json_backend_state current;
    enum json_backend_state next;
    const char* str;
} MAP[] = {
    { .current = STATE_MSG, .next = STATE_UID,      .str = UID          },
    { .current = STATE_MSG, .next = STATE_TYPE,     .str = TYPE         },
    { .current = STATE_MSG, .next = STATE_COMMANDS, .str = COMMANDS     },
    { .current = STATE_MSG, .next = STATE_FLAGS,    .str = FLAGS        },
    {
        .current = STATE_FLAGS_MAP,
        .next = STATE_FLAGS_EXEC,
        .str = FLAG_EXEC
    },
    {
        .current = STATE_FLAGS_MAP,
        .next = STATE_FLAGS_REGISTER,
        .str = FLAG_REGISTER
    },
    {
        .current = STATE_MSG,
        .next = STATE_EVENT_NAME,
        .str = EVENT_NAME
    },
    {
        .current = STATE_MSG,
        .next = STATE_EVENT_VALUE,
        .str = EVENT_VALUE,
    },

    { .str = NULL },
};

static struct ws_logger_context log_ctx = {
    .prefix = "[JSON Deserializer, YAJL interface] ",
};


/*
 *
 * static function definitions
 *
 */

/**
 * Setup the deserializer_state object to hold a transaction
 *
 * @return zero on success, else negative errno.h number
 */
int
setup_transaction(
    struct ws_deserializer* s // The current state object
);

/**
 * Finalize the message and clear the buffers we still have lying around
 */
void
finalize_message(
    struct ws_deserializer* d //!< The deserializer obj, containing everything
);

/**
 * Get the next state for the current state and a string
 */
static enum json_backend_state
get_next_state_for_string(
    enum json_backend_state current,
    const unsigned char * str
);

/*
 *
 * Interface implementation
 *
 */

int
yajl_null_cb(
    void* ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: NULL");

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            ws_log(&log_ctx, LOG_DEBUG,
                   "Appending as Command argument (directly)");

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

    case STATE_EVENT_VALUE:
        // event value is NULL
        {
            ws_log(&log_ctx, LOG_DEBUG, "Using as event value");

            state->has_event = true;
            struct ws_value_nil* nil = calloc(1, sizeof(*nil));
            if (!nil) {
                //!< @tod error, what now?
                return 0;
            }
            ws_value_nil_init(nil);
            state->ev_ctx = (struct ws_value*) nil;

            state->current_state = STATE_MSG;
        }
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;

    }
    return 1;
}

int
yajl_boolean_cb(
    void* ctx,
    int b
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Bool (%i)", b);

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            ws_log(&log_ctx, LOG_DEBUG,
                   "Appending as Command argument (directly)");
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

    case STATE_FLAGS_EXEC:
        ws_log(&log_ctx, LOG_DEBUG, "Using as transaction exec-flag");
        if (b) {
            state->flags |= WS_TRANSACTION_FLAGS_EXEC; // set
        } else {
            state->flags &= ~WS_TRANSACTION_FLAGS_EXEC; // unset
        }
        state->current_state = STATE_FLAGS_MAP;
        break;

    case STATE_EVENT_VALUE:
        // event value is a boolean
        {
            ws_log(&log_ctx, LOG_DEBUG, "Using as event value");
            state->has_event = true;
            struct ws_value_bool* boo = calloc(1, sizeof(*boo));
            if (!b) {
                //!< @tod error, what now?
                return 0;
            }
            ws_value_bool_init(boo);
            ws_value_bool_set(boo, b);
            state->ev_ctx = (struct ws_value*) boo;

            state->current_state = STATE_MSG;
        }
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_integer_cb(
    void* ctx,
    long long i
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Integer (%lld)", i);

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_UID:
        ws_log(&log_ctx, LOG_DEBUG, "Using as UID");
        if (d->buffer) {
            // Hey, we have a message object, set the ID directly
            d->buffer->id = i; //!< @todo visibility violation here
        } else {
            // cache the ID
            state->id = i;
        }
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            ws_log(&log_ctx, LOG_DEBUG, "Using as direct argument");
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

    case STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT_STACKPOS:
        ws_log(&log_ctx, LOG_DEBUG, "Using as indirect stack position");
        ws_statement_append_indirect(state->tmp_statement, i);

        // Keep the state for now.
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT_STACKPOS;
        break;

    case STATE_COMMAND_ARY_COMMAND_NAME:
        ws_log(&log_ctx, LOG_DEBUG,
               "Using as counter for number of implicit arguments");
        state->tmp_statement->args.num = i;
        state->tmp_statement->args.vals = NULL;

        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    case STATE_EVENT_VALUE:
        // event value is an integer
        {
            ws_log(&log_ctx, LOG_DEBUG, "Using as event value");
            state->has_event = true;
            struct ws_value_int* n = calloc(1, sizeof(*n));
            if (!n) {
                //!< @tod error, what now?
                return 0;
            }
            ws_value_int_init(n);
            ws_value_int_set(n, i);
            state->ev_ctx = (struct ws_value*) n;

            state->current_state = STATE_MSG;
        }
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_double_cb(
    void* ctx,
    double d
) {
    //!< @todo implement
    return 0;
}

int
yajl_string_cb(
    void* ctx,
    const unsigned char* str,
    size_t len
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: String (<unterminated>)");

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_INIT:
        // a strange thing happened
        return 0;

    case STATE_TYPE:
        ws_log(&log_ctx, LOG_DEBUG, "Using as type identifier");
        if (0 == strncmp(TYPE_TRANSACTION, (char*) str,
                    strlen(TYPE_TRANSACTION))) {
            setup_transaction(d);
        } else if (0 == strncmp(TYPE_EVENT, (char*) str, strlen(TYPE_EVENT))) {
            state->has_event = true;
        }

        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        {
            char buff[len + 1];
            strncpy(buff, (char*) str, len);
            buff[len] = 0;
            ws_log(&log_ctx, LOG_DEBUG, "Using as argument (%s)", buff);

            struct ws_value_string* s = ws_value_string_new();
            if (!s) {
                return 0;
            }
            struct ws_string* str = ws_value_string_get(s);
            int res = ws_string_set_from_raw(str, buff);
            if (res != 0) {
                //!< @todo indicate error
                return 0;
            }

            if (0 != ws_statement_append_direct(state->tmp_statement,
                                                (struct ws_value*) s)) {
                //!< @todo error
                return 0;
            }

            state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        }
        break;

    case STATE_FLAGS_REGISTER:
        {
            state->register_name = ws_string_new();
            if (!state->register_name) {
                //!< @todo error, what now?
                return 0;
            }

            char buff[len + 1];
            strncpy(buff, (char*)str, len);
            buff[len] = 0;
            ws_log(&log_ctx, LOG_DEBUG,
                   "Using as identifier for registration (%s)", buff);

            int res = ws_string_set_from_raw(state->register_name, buff);
            if (res != 0) {
                //!< @todo indicate error
                return 0;
            }

            state->flags |= WS_TRANSACTION_FLAGS_REGISTER;
            state->current_state = STATE_FLAGS_MAP;
        }
        break;

    case STATE_EVENT_NAME:
        // This is the name of the event we are deserializing
        {
            state->has_event = true;
            state->ev_name = ws_string_new();
            if (!state->ev_name) {
                //!< @todo error, what now?
                return 0;
            }

            char buff[len + 1];
            strncpy(buff, (char*) str, len);
            buff[len] = 0;
            ws_log(&log_ctx, LOG_DEBUG, "Using as event name (%s)", buff);

            int res = ws_string_set_from_raw(state->ev_name, buff);
            if (res != 0) {
                //!< @todo indicate error
                return 0;
            }

            // ready for now.

            state->current_state = STATE_MSG;
        }
        break;

    case STATE_EVENT_VALUE:
        // event value is a string
        {
            state->has_event = true;
            struct ws_value_string* s = ws_value_string_new();
            if (!s) {
                //!< @tod error, what now?
                return 0;
            }

            struct ws_string* sstr = ws_value_string_get(s);

            char buff[len + 1];
            strncpy(buff, (char*) str, len);
            buff[len] = 0;
            ws_log(&log_ctx, LOG_DEBUG, "Using as event value (%s)", buff);

            int res = ws_string_set_from_raw(sstr, buff);
            if (res != 0) {
                //!< @todo indicate error
                return 0;
            }

            state->ev_ctx = (struct ws_value*) s;

            state->current_state = STATE_MSG;
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
    void* ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Map-Open");

    state->ncurvedbrackets++;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_INIT:
        ws_log(&log_ctx, LOG_DEBUG, "Starting message");
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY:
        // We run into a new command here
        ws_log(&log_ctx, LOG_DEBUG, "Starting new command");
        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        ws_log(&log_ctx, LOG_DEBUG, "Starting new direct command argument");
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARG_DIRECT;
        break;

    case STATE_FLAGS:
        ws_log(&log_ctx, LOG_DEBUG, "Starting flag map");
        state->current_state = STATE_FLAGS_MAP;
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_map_key_cb(
    void* ctx,
    const unsigned char* key,
    size_t len
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Map-Key (<unterminated>)");

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_MSG:
    case STATE_FLAGS_MAP:
        // We're running into a top-level key here, lets decide what comes next:
        ws_log(&log_ctx, LOG_DEBUG, "Using as top-level key");
        state->current_state = get_next_state_for_string(state->current_state,
                                                         key);
        break;

    case STATE_COMMAND_ARY_NEW_COMMAND:
        // This is now the KEY of the command, the command name.
        // The next state should be the command argument array
        {
            char buf[len + 1];
            strncpy(buf, (char*) key, len);
            buf[len] = 0;
            ws_log(&log_ctx, LOG_DEBUG, "Using as command name (%s)", buf);

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
            ws_log(&log_ctx, LOG_DEBUG, "Invalid, expected position key");
            state->current_state = STATE_INVALID;
            break;
        }
        ws_log(&log_ctx, LOG_DEBUG, "Using as key for stack position argument");
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT_STACKPOS;
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_end_map_cb(
    void* ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Map-End");

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
        ws_log(&log_ctx, LOG_DEBUG, "Finished command");
        break;

    case STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT_STACKPOS:
        ws_log(&log_ctx, LOG_DEBUG, "Finished command arg: Indirect stack pos");
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        ws_log(&log_ctx, LOG_DEBUG, "Finished command arguments");
        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    case STATE_FLAGS_MAP:
        ws_log(&log_ctx, LOG_DEBUG, "Finished flags");
        state->current_state = STATE_MSG;
        break;

    case STATE_MSG:
        ws_log(&log_ctx, LOG_DEBUG, "Finished message");
        state->current_state = STATE_INIT;
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    if (state->nboxbrackets == 0 && state->ncurvedbrackets == 0) {
        // Hey, we are ready now!
        d->is_ready = true;
        finalize_message(d);
        return 0;
    }
    return 1;
}

int
yajl_start_array_cb(
    void* ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Array-Start");

    state->nboxbrackets++;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMANDS:
        ws_log(&log_ctx, LOG_DEBUG, "Start command array");
        setup_transaction(d);
        state->current_state = STATE_COMMAND_ARY;
        break;

    case STATE_COMMAND_ARY_COMMAND_NAME:
        ws_log(&log_ctx, LOG_DEBUG, "Start command arguments");
        // We are in the command name state and the next thing is an array, so
        // we must allocate the command argument array here.
        state->current_state = STATE_COMMAND_ARY_COMMAND_ARGS;
        break;

    default:
        ws_log(&log_ctx, LOG_DEBUG, "INVALID");
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

int
yajl_end_array_cb(
    void* ctx
) {
    struct ws_deserializer* d = (struct ws_deserializer*) ctx;
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    ws_log(&log_ctx, LOG_DEBUG, "Detected: Array-End");

    state->nboxbrackets--;

    switch (state->current_state) {
    case STATE_INVALID:
        return 1;

    case STATE_COMMAND_ARY:
        // We are ready with the command array parsing now.
        state->current_state = STATE_MSG;
        break;

    case STATE_COMMAND_ARY_COMMAND_ARGS:
        // The command argument array closes now, we are ready with the command
        // argument array parsing here. So we go back to the "new command"
        // state. The finalization is done after the map for the new command
        // closes.

        state->current_state = STATE_COMMAND_ARY_NEW_COMMAND;
        break;

    default:
        state->current_state = STATE_INVALID;
        break;
    }

    return 1;
}

/*
 *
 * static function implementations
 *
 */

int
setup_transaction(
    struct ws_deserializer* self
) {
    if (!self) {
        return -EINVAL;
    }

    if (self->buffer != NULL) {
        return 0;
    }

    //!< @todo assign real name, flags
    enum ws_transaction_flags flags = WS_TRANSACTION_FLAGS_EXEC;
    self->buffer = (struct ws_message*) ws_transaction_new(0, NULL,
                                                           flags, NULL);

    return 0;
}

void
finalize_message(
    struct ws_deserializer* d
) {
    struct deserializer_state* state = (struct deserializer_state*) d->state;

    if (d->buffer == NULL && !state->has_event) {
        // Do runtime check whether buffer object exists here, because _someone_
        // decided against runtime checks in the utility functions
        return;
    }

    if (d->buffer) {
        if (ws_object_is_instance_of((struct ws_object*) d->buffer,
                                     &WS_OBJECT_TYPE_ID_TRANSACTION)) {

            struct ws_transaction* t = (struct ws_transaction*) d->buffer;

            ws_transaction_set_flags(t, state->flags);

            // gets a ref on name
            ws_transaction_set_name(t, state->register_name);
            ws_object_unref((struct ws_object*) state->register_name);

            return;
        }
    }

    if (state->has_event) {
        d->buffer = (struct ws_message*) ws_event_new(state->ev_name,
                                                      state->ev_ctx);
        return;
    }
}

static enum json_backend_state
get_next_state_for_string(
    enum json_backend_state current,
    const unsigned char* str
) {
    for (size_t i = 0; MAP[i].str; i++) {
        if (MAP[i].current == current) {
            if (0 == strncmp(MAP[i].str, (char*) str, strlen(MAP[i].str))) {
                return MAP[i].next;
            }
        }
    }

    return STATE_INVALID;
}
