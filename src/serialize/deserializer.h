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

#ifndef __WS_SERIALIZE_DESERIALIZER_H__
#define __WS_SERIALIZE_DESERIALIZER_H__

#include "util/attributes.h"

#include <malloc.h>
#include <stdbool.h>

// Forward declarations
struct ws_deserializer;
struct ws_message;


/**
 * The deserialization callback takes the deserializer itself, a pointer to
 * the data which may be deserialized (`buf`) and the length of the buffer
 * (`nbuf`).
 *
 * The function should deserialize a single message, update `nbuf` to the bytes
 * processed as well as the messagei, set the `is_ready` flag and return the
 * number of bytes read from the buffer.
 * If the buffer didn't contain a complete message, the function should try to
 * deserialize as much of the message as possible, updating the internal state
 * and return the number of bytes read, not setting the `is_ready` flag.
 * If the message was malformed in a non-recoverable way, the function should
 * return `-EPROTO`.
 * The function may also return a negative value if it encounters an internal
 * error.
 */
typedef ssize_t (*ws_deserialize_f)(struct ws_deserializer* self,
                                     char const* buf, size_t nbuf);


/**
 * Deserializer type
 *
 * @attention This type is abstract
 *
 * A deserializer is initialized by a preparing function, which sets the
 * callbacks and initializes the internal state to be used for parsing.
 */
struct ws_deserializer {
    ws_deserialize_f deserialize; //!< @protected deserialisation function
    void (*deinit)(void*); //!< @protected deinitialize the internal state
    void* state; //!< @protected internal state of the deserializer
    struct ws_message* buffer; //!< @protected construction and handover buffer
    bool is_ready; //!< @protected flag indicating completeness of the message
};

/**
 * Deserialize a message
 *
 * This function tries to deserialize a message.
 * It takes a buffer (and it's length) as the source for the deserialisation and
 * will return either the number of bytes read from that buffer or a negative
 * error code.
 * `msg` will be set either to the message which was extracted or to `NULL`, if
 * no message could be extracted.
 *
 * @return the number of bytes processed of a negative error code
 *
 */
ssize_t
ws_deserialize(
    struct ws_deserializer* self, //!< the deserializer
    struct ws_message** msg, //!< return pointer for the message
    char const* buf, //!< the buffer containing the message to deserialize
    size_t nbuf //!< the length of the buffer
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Deinitialize the deserializer
 *
 * This function deinitializes the deserializer
 */
void
ws_deserializer_deinit(
    struct ws_deserializer* self //!< the deserializer
)
__ws_nonnull__(1)
;


#endif //__WS_SERIALIZE_DESERIALIZER_H__

