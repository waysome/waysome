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

#include "objects/message/error_reply.h"
#include "objects/message/transaction.h"
#include "values/value.h"


/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialize an error reply message
 *
 * @return true
 */
bool
error_reply_deinit(
    struct ws_object* obj
);


/*
 *
 * Internal constants
 *
 */

/**
 * Type id of the `ws_error_reply` type.
 */
ws_object_type_id WS_OBJECT_TYPE_ID_ERROR_REPLY = {
    .supertype  = &WS_OBJECT_TYPE_ID_REPLY,
    .typestr    = "ws_reply",

    .hash_callback = NULL,
    .deinit_callback = NULL,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .function_table = NULL,
};


/*
 *
 * Interface implementation
 *
 */

struct ws_error_reply*
ws_error_reply_new(
    struct ws_transaction* src,
    unsigned int code,
    char const* description,
    char const* cause
) {
    struct ws_error_reply* retval = calloc(1, sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    if (ws_message_init((struct ws_message*) retval, src->m.id) < 0) {
        free(retval);
        return NULL;
    }
    ((struct ws_object*) retval)->id = &WS_OBJECT_TYPE_ID_ERROR_REPLY;
    ((struct ws_object*) retval)->settings = WS_OBJECT_HEAPALLOCED;

    // optimistical writes
    retval->code        = code;
    retval->description = NULL;
    retval->cause       = NULL;

    if (description) {
        retval->description = strdup(description);
    }

    if (cause) {
        retval->cause = strdup(cause);
    }

    return retval;
}

unsigned int
ws_error_reply_get_code(
    struct ws_error_reply* self //!< error reply from which to extract the code
) {
    return self->code;
}

char const*
ws_error_reply_get_description(
    struct ws_error_reply* self //!< error reply from which to extract the code
) {
    if (self->description) {
        return self->description;
    }
    return "";
}

char const*
ws_error_reply_get_cause(
    struct ws_error_reply* self //!< error reply from which to extract the code
) {
    if (self->cause) {
        return self->cause;
    }
    return "";
}

/*
 *
 * Internal implementation
 *
 */

bool
error_reply_deinit(
    struct ws_object* obj
) {
    struct ws_error_reply* reply = (struct ws_error_reply*) obj;
    free((char*) reply->description);
    free((char*) reply->cause);

    return true;
}

