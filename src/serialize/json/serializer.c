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
 * @file json/serializer.c
 *
 * This file contains the serializer backend for JSON.
 *
 * The implementation makes use of the yajl library. For a documentation of the
 * yajl library, see: https://lloyd.github.io/yajl/
 */

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#include "logger/module.h"
#include "objects/message/error_reply.h"
#include "objects/message/event.h"
#include "objects/message/message.h"
#include "objects/message/value_reply.h"
#include "objects/string.h"
#include "serialize/json/keys.h"
#include "serialize/json/serializer.h"
#include "serialize/json/serializer_state.h"
#include "serialize/serializer.h"
#include "util/arithmetical.h"
#include "util/condition.h"
#include "values/object_id.h"
#include "values/value.h"

static struct ws_logger_context log_ctx = {
    .prefix = "[Serializer/JSON] ",
};

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

/**
 * Serialize a value
 */
static int
serialize_value(
    struct serializer_context* ctx,
    struct ws_value* val
);

/**
 * Serialize callback for ws_value_set_select()
 *
 * @note must be casted to (int (*)(void*, void const*)) to be able to pass to
 * ws_set_select()
 */
static int
serialize_object_to_id_string(
    struct serializer_context* ctx,
    struct ws_object* obj
);

/**
 * Generate a key in the buffer
 *
 * Generates a new key in the buffer, increments the value behind `nbytes`
 * afterwards and sets the next state in the context.
 *
 * @return zero on success, else negative errno.h number
 */
static int
gen_key(
    struct serializer_context* ctx,
    char const* str
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

    ser->buffer     = NULL;
    ser->serialize  = serialize;
    ser->deinit     = NULL;

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

    struct serializer_context* ctx = (struct serializer_context*) self->state;
    if (ctx->current_state == STATE_NO_STATE ||
            ctx->current_state == STATE_INIT_STATE) {
        // We are starting with parsing right now.
        ctx->current_state = STATE_INIT_STATE; // if in NO_STATE

        yajl_gen_status stat = yajl_gen_map_open(ctx->yajlgen);
        if (stat != yajl_gen_status_ok) {
            ws_log(&log_ctx, LOG_DEBUG, "Error opening main map");
            return -1;
        }

        ctx->current_state = STATE_MESSAGE_STATE;
    }

    if (ctx->current_state == STATE_READY) {
        goto write_buffer;
    }

    {
        bool serialized = false;
        /*
         * Map from type -> serializing function
         */
        static const struct {
            ws_object_type_id* type;
            int (*serf)(struct ws_serializer*);
        } FUNC_TAB[] = {
            {
                .type = &WS_OBJECT_TYPE_ID_EVENT,
                .serf = serialize_event
            },
            {
                .type = &WS_OBJECT_TYPE_ID_ERROR_REPLY,
                .serf = serialize_reply_error_reply,
            },
            {
                .type = &WS_OBJECT_TYPE_ID_VALUE_REPLY,
                .serf = serialize_reply_value_reply,
            }
        };

        for (size_t i = 0; i < ARYLEN(FUNC_TAB); ++i) {
            if (FUNC_TAB[i].type != self->buffer->obj.id) {
                continue;
            }

            int retval = FUNC_TAB[i].serf(self);
            if (unlikely(retval < 0)) {
                char const* name = ws_object_typename(&self->buffer->obj);
                ws_log(&log_ctx, LOG_DEBUG,
                       "Serializing error when serializing '%s'", name);
                return retval;
            }
            serialized = true;
            break;
        }

        if (!serialized) {
            return -EINVAL;
        }
    }

    { // lets close the main map now.
        yajl_gen_status stat = yajl_gen_map_close(ctx->yajlgen);
        if (unlikely(stat != yajl_gen_status_ok)) {
            ws_log(&log_ctx, LOG_DEBUG, "Error closing main map");
            return -1;
        }
    }

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

        //!< @todo do I need to free the self->buffer object here?
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
    struct ws_error_reply* r = (struct ws_error_reply*) self->buffer;
    struct serializer_context* ctx = (struct serializer_context*) self->state;

    if (unlikely(gen_key(ctx, ERROR_CODE))) {
        return -1;
    }

    yajl_gen_status stat = yajl_gen_integer(ctx->yajlgen,
                                            ws_error_reply_get_code(r));
    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error generating int for error reply");
        return -1;
    }

    if (unlikely(gen_key(ctx, ERROR_DESC))) {
        return -1;
    }

    char const* tmp = ws_error_reply_get_description(r);
    stat = yajl_gen_string(ctx->yajlgen, (unsigned char*) tmp, strlen(tmp));
    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error generating desc: '%s'", tmp);
        return -1;
    }

    if (unlikely(gen_key(ctx, ERROR_CAUSE))) {
        return -1;
    }

    tmp = ws_error_reply_get_cause(r);
    stat = yajl_gen_string(ctx->yajlgen, (unsigned char*) tmp, strlen(tmp));
    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error generating cause: '%s'", tmp);
        return -1;
    }

    return 0;
}

static int
serialize_reply_value_reply(
    struct ws_serializer* self
) {
    struct serializer_context* ctx = (struct serializer_context*) self->state;

    // We haven't serialized anything
    // generate the key for the value reply
    if (unlikely(gen_key(ctx, VALUE))) {
        return -1;
    }

    struct ws_value* v = &((struct ws_value_reply*) self->buffer)->value.value;
    if (serialize_value(ctx, v)) {
        return -1;
    }

    if (unlikely(gen_key(ctx, TRANSACTION_ID))) {
        return -1;
    }

    size_t id = ws_message_get_id(self->buffer);
    yajl_gen_status stat = yajl_gen_integer(ctx->yajlgen, id);
    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error serializing msg id: %i", id);
        return -1;
    }

    return 0;
}

static int
serialize_event(
    struct ws_serializer* self
) {
    struct serializer_context* ctx = (struct serializer_context*) self->state;

    // We haven't serialized anything
    // generate the key for the event message
    if (gen_key(ctx, EVENT)) {
        //!< @todo error?
        return -1;
    }
    // We also know already what we have to serialize, so... let's try it

    // We have a JSON Object "event"
    yajl_gen_status stat = yajl_gen_map_open(ctx->yajlgen);
    if (stat != yajl_gen_status_ok) {
        //!< @todo error?
        return -1;
    }

    // We have a '{ "event" : {' in the buffer by now
    if (unlikely(gen_key(ctx, EVENT_CTX))) {
        return -1;
    }

    struct ws_event* ev = (struct ws_event*) self->buffer;

    // We have a '{ "event" : { "context" : ' in the buffer by now
    if (serialize_value(ctx, &ev->context.value) != 0) {
        //!< @todo error?
        return -1;
    }

    // We have a '{ "event" : { <context:map> ' in the buffer by now
    if (unlikely(gen_key(ctx, EVENT_NAME))) {
        return -1;
    }

    // We have a
    //  '{ "event" : { <context:map>, "name": '
    // in the buffer by now
    {
        char* plain = ws_string_raw(&ev->name);
        size_t len = ws_string_len(&ev->name);

        stat = yajl_gen_string(ctx->yajlgen, (unsigned char*) plain, len);
        if (stat != yajl_gen_status_ok) {
            //!< @todo error?
            return -1;
        }
    }

    // lets close the event map now.
    stat = yajl_gen_map_close(ctx->yajlgen);
    if (stat != yajl_gen_status_ok) {
        //!< @todo error?
        return -1;
    }

    ctx->current_state = STATE_READY;
    return 0;
}

static int
serialize_value(
    struct serializer_context* ctx,
    struct ws_value* val
) {
    yajl_gen_status stat;

    switch (ws_value_get_type(val)) {
    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
    case WS_VALUE_TYPE_NIL:
        stat = yajl_gen_null(ctx->yajlgen);
        break;
    case WS_VALUE_TYPE_BOOL:
        {
            bool b = ws_value_bool_get((struct ws_value_bool*) val);
            stat = yajl_gen_bool(ctx->yajlgen, b);
        }
        break;

    case WS_VALUE_TYPE_INT:
        {
            int i = ws_value_int_get((struct ws_value_int*) val);
            stat = yajl_gen_integer(ctx->yajlgen, i);
        }
        break;

    case WS_VALUE_TYPE_STRING:
        {
            struct ws_string* str;
            str = ws_value_string_get((struct ws_value_string*) val);

            char* buf = ws_string_raw(str); // buf is a copy
            ws_object_unref((struct ws_object*) str);

            stat = yajl_gen_string(ctx->yajlgen, (unsigned char*) buf,
                                   strlen(buf));
        }
        break;

    default:
        {
            stat = yajl_gen_map_open(ctx->yajlgen);
            if (stat != yajl_gen_status_ok) {
                ws_log(&log_ctx, LOG_DEBUG, "Error opening map");
                return -1;
            }

            if (unlikely(gen_key(ctx, ws_value_type_get_name(val)))) {
                ws_log(&log_ctx, LOG_DEBUG, "Error generating map key");
                return -1;
            }

            switch (ws_value_get_type(val)) {
            case WS_VALUE_TYPE_OBJECT_ID:
                { // value for object-id JSON object
                    struct ws_value_object_id* obj_id;
                    obj_id = (struct ws_value_object_id*) val;
                    struct ws_object* object = ws_value_object_id_get(obj_id);
                    if (serialize_object_to_id_string(ctx, object)) {
                        //!< @todo error?
                        return -1;
                    }
                }
                break;

            case WS_VALUE_TYPE_SET:
                stat = yajl_gen_array_open(ctx->yajlgen);
                if (stat != yajl_gen_status_ok) {
                    ws_log(&log_ctx, LOG_DEBUG, "Error opening array");
                    return -1;
                }

                //!< @todo check return value ... might return before ready
                ws_value_set_select((struct ws_value_set*) val, NULL, NULL,
                        (int (*)(void*, void const*))
                            serialize_object_to_id_string,
                        ctx);

                stat = yajl_gen_array_close(ctx->yajlgen);
                break;

            default:
                {
                    const char* ty = ws_value_type_get_name(val);
                    ws_log(&log_ctx, LOG_DEBUG,
                           "Unable to serialize '%s' type", ty);
                }
                return -1;
            }

            if (unlikely(stat != yajl_gen_status_ok)) {
                const char* ty = ws_value_type_get_name(val);
                ws_log(&log_ctx, LOG_DEBUG,
                       "Unable to serialize '%s' type", ty);
                return -1;
            }

            stat = yajl_gen_map_close(ctx->yajlgen);
        }
    }

    if (stat != yajl_gen_status_ok) {
        const char* vname = ws_value_type_get_name(val);
        ws_log(&log_ctx, LOG_DEBUG, "Error serializing value: '%s'", vname);
        return -1;
    }

    return 0;
}

static int
serialize_object_to_id_string(
    struct serializer_context* ctx,
    struct ws_object* obj
) {
    uintmax_t uuid = ws_object_uuid(obj);
    size_t bufflen = (sizeof(uuid) << 1) + 1;
    char buff[bufflen];

    snprintf(buff, bufflen, "%"PRIxMAX, uuid);

    yajl_gen_status stat = yajl_gen_string(ctx->yajlgen,
                                           (unsigned char*) buff,
                                           bufflen - 1);
    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error serializing id string");
        return -1;
    }

    return 0;
}

static int
gen_key(
    struct serializer_context* ctx,
    char const* str
) {
    yajl_gen_status stat = yajl_gen_string(ctx->yajlgen,
                                           (const unsigned char*) str,
                                           strlen(str));

    if (unlikely(stat != yajl_gen_status_ok)) {
        ws_log(&log_ctx, LOG_DEBUG, "Error generating key: '%s'", str);
        return -EAGAIN;
    }

    return 0;
}

