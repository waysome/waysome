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

#ifndef __WS_SERIALIZE_SERIALIZER_H__
#define __WS_SERIALIZE_SERIALIZER_H__

#include "util/attributes.h"

#include <malloc.h>

// Forward declarations
struct ws_serializer;
struct ws_message;


/**
 * The serialization callback takes the serializer itself, a pointer to
 * a target buffer to serialize to (`buf`), the length of the buffer in bytes
 * (`nbuf`).
 * The function should serialize the message stored in the serializer, `unref`
 * the message and set the corresponding pointer of the serializer to `NULL` and
 * finally return the number of bytes written.
 * If the buffer's length is not sufficient for writing the message, as much of
 * as possible should be written and the function should update the internal
 * state in way that future invocations can continue serialisation, not freeing
 * the message.
 * If an internal error occurred, a negative error value may be returned.
 */
typedef ssize_t (*ws_serialiize_f)(struct ws_serializer* self,
                                   char* buf, size_t nbuf);


/**
 * Deserializer type
 *
 * A deserializer is initialized by a preparing function, which sets the
 * callbacks and initializes the internal state to be used for parsing.
 */
struct ws_serializer {
    ws_serialiize_f serialize; //!< serialization function
    void (*deinit)(void*); //!< deinitialize the internal state
    void* state; //!< internal state of the serializer
    struct ws_message* buffer; //!< storage for an incompletely written message
};

/**
 * Serialize a message
 *
 * This function tries to serialize a message.
 * It takes a buffer (and it's length) as the target for serialisation and will
 * return either the number of bytes written to that buffer or a negative error
 * code.
 * Note that in the case that the message could not be serialized because
 * another message is still pending, `-EAGAIN` will be returned, indicating that
 * the message may be serialized successfully later.
 *
 */
ssize_t
ws_serialize(
    struct ws_serializer* self, //!< the serializer
    char* buf, //!< the target buffer for the operation
    size_t nbuf, //!< the length of the buffer available
    struct ws_message* msg //!< message to serialize
)
__ws_nonnull__(1, 2, 4)
;

/**
 * Deinitialize the serializer
 *
 * This function deinitializes the serializer
 */
void
ws_serializer_deinit(
    struct ws_serializer* self //!< the serializer
)
__ws_nonnull__(1)
;

#endif //__WS_SERIALIZE_SERIALIZER_H__

