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

#include "objects/message/transaction.h"
#include "objects/message/value_reply.h"

/*
 *
 * Forward declarations
 *
 */

bool
value_reply_deinit(
    struct ws_object* obj
);

/*
 *
 * Internal constants
 *
 */

/**
 * Type id of the `ws_value_reply` type.
 */
ws_object_type_id WS_OBJECT_TYPE_ID_VALUE_REPLY = {
    .supertype  = &WS_OBJECT_TYPE_ID_REPLY,
    .typestr    = "ws_reply",

    .hash_callback = NULL,
    .deinit_callback = value_reply_deinit,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .function_table = NULL,
};


/*
 *
 * Interface implementation
 *
 */

struct ws_value_reply*
ws_value_reply_new(
    struct ws_transaction* src,
    struct ws_value* value
) {
    struct ws_value_reply* retval = calloc(1, sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    int res;

    res = ws_message_init((struct ws_message*) retval, src->m.id);
    if (res < 0) {
        goto cleanup;
    }
    ((struct ws_object*) retval)->id = &WS_OBJECT_TYPE_ID_VALUE_REPLY;
    ((struct ws_object*) retval)->settings = WS_OBJECT_HEAPALLOCED;

    if (value) {
        res = ws_value_union_init_from_val(&retval->value, value);
        if (res < 0) {
            goto cleanup;
        }
    } else {
        ws_value_nil_init(&retval->value.nil);
    }

    return retval;

cleanup:
    free(retval);
    return NULL;
}

struct ws_value*
ws_value_reply_get_value(
    struct ws_value_reply* self
) {
    // No locking here as the value of the value reply shouldn't change
    return &self->value.value;
}


/*
 *
 * Internal implementation
 *
 */

bool
value_reply_deinit(
    struct ws_object* obj
) {
    struct ws_value_reply* reply = (struct ws_value_reply*) obj;
    ws_value_deinit(&reply->value.value);
    return true;
}

