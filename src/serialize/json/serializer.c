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
 * @file serializer.c
 *
 * This file contains the serializer backend for JSON.
 *
 * The implementation makes use of the yajl library. For a documentation of the
 * yajl library, see: https://lloyd.github.io/yajl/
 */

#include <errno.h>
#include <stdlib.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#include "objects/message/error_reply.h"
#include "objects/message/event.h"
#include "objects/message/message.h"
#include "objects/message/value_reply.h"
#include "serialize/json/serializer.h"
#include "serialize/json/serializer_state.h"
#include "serialize/serializer.h"
#include "util/arithmetical.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * serialize() callback
 *
 * @return negative errno.h number on failure, else the number of written bytes
 */
static ssize_t
serialize(
    struct ws_serializer* self,
    char* buf,
    size_t nbuf
);

/**
 * Serialize an error reply type
 *
 * @return zero on success, else negative errno.h number
 */
static int
serialize_reply_error_reply(
    struct ws_serializer* self
);

/**
 * Serialize a value reply type
 *
 * @return zero on success, else negative errno.h number
 */
static int
serialize_reply_value_reply(
    struct ws_serializer* self
);

/**
 * Serialize an event type
 *
 * @return zero on success, else negative errno.h number
 */
static int
serialize_event(
    struct ws_serializer* self
);

/*
 *
 * Interface implementation
 *
 */

struct ws_serializer*
ws_serializer_json_serializer_new(void)
{
    struct ws_serializer* ser = calloc(1, sizeof(*ser));

    if (!ser) {
        return NULL;
    }

    ser->state = serializer_context_new();

    if (!ser->state) {
        free(ser);
        return NULL;
    }

    ser->buffer = NULL;
    ser->serialize = serialize;
    ser->deinit = NULL;

    return ser;
}

/*
 *
 * Static function implementations
 *
 */

static ssize_t
serialize(
    struct ws_serializer* self,
    char* buf,
    size_t nbuf
) {
    if (!self->buffer) {
        return -ENOENT;
    }

    ws_object_type_id* type = self->buffer->obj.id;

    struct serializer_context* ctx = (struct serializer_context*) self->state;
    if (ctx->current_state == STATE_NO_STATE ||
            ctx->current_state == STATE_INIT_STATE) {
        // We are starting with parsing right now.
        ctx->current_state = STATE_INIT_STATE; // if in NO_STATE

        yajl_gen_status stat = yajl_gen_map_open(ctx->yajlgen);

        if (stat != yajl_gen_status_ok) {
            //!< @todo error opening map, what to do now?
            return -1;
        }

        ctx->current_state = STATE_MESSAGE_STATE;
    }

    if (ctx->current_state == STATE_MESSAGE_READY) {
        goto write_buffer;
    }

    do {
        int retval = 0;
        if (type == &WS_OBJECT_TYPE_ID_EVENT) {
            retval = serialize_event(self);
            if (retval < 0) {
                //!< @todo something went wrong serializing the event. Error?
                return retval;
            }
            break;
        }

        if (type == &WS_OBJECT_TYPE_ID_ERROR_REPLY) {
            retval = serialize_reply_error_reply(self);
            if (retval < 0) {
                //!< @todo something went wrong serializing the error. Error?
                return retval;
            }
            break;
        }

        if (type == &WS_OBJECT_TYPE_ID_VALUE_REPLY) {
            retval = serialize_reply_value_reply(self);
            if (retval < 0) {
                //!< @todo something went wrong serializing the reply. Error?
                return retval;
            }
            break;
        }

        return -EINVAL;
    } while (0);

write_buffer:
    // put the serilized stuff into the buffer
    if (ctx->yajl_buffer == NULL) {
        yajl_gen_get_buf(ctx->yajlgen,
                         (const unsigned char**) &ctx->yajl_buffer,
                         &ctx->yajl_buffer_size);
    }

    size_t write = MIN(ctx->yajl_buffer_size, nbuf);
    strncpy(buf, (char*) ctx->yajl_buffer, write);

    if (ctx->yajl_buffer_size <= nbuf) {
        // We are ready now, as the yajl buffer is smaller or of equal size
        // as the buffer we just wrote to
        self->buffer = NULL; // "I am ready here!"
    } else {
        // We must wait until we get a buffer where we can write the rest
        // to.
        ctx->yajl_buffer        += MIN(ctx->yajl_buffer_size, nbuf);
        ctx->yajl_buffer_size   -= MIN(ctx->yajl_buffer_size, nbuf);
    }

    return write;
}

static int
serialize_reply_error_reply(
    struct ws_serializer* self
) {
    //!< @todo implement
    return -1;
}

static int
serialize_reply_value_reply(
    struct ws_serializer* self
) {
    //!< @todo implement
    return -1;
}

static int
serialize_event(
    struct ws_serializer* self
) {
    //!< @todo implement
    return -1;
}
