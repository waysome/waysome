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

#include <stdlib.h>

#include "serialize/json/serializer.h"

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
    //!< @todo implement
    return NULL;
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
    //!< @todo implement
    return -1;
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
