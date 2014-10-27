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
 * @addtogroup connbuf "connection buffer"
 *
 * @{
 */

#ifndef __WS_CONNBUF_H__
#define __WS_CONNBUF_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * Connection buffer
 *
 * When `read()`ing and `write()`ing to a connection, a programmer has to take
 * care of the buffering of incomplete content.
 *
 * E.g. a series of bytes read from a file descriptor may or may not contain
 * a message which may be deserialized by a parser in one run.
 * If a streaming parser is used, residual content, which may be parsed some
 * time in the future, may be left in the buffer.
 * Similarly, a `write()` may not be able to send every last byte of the buffer.
 *
 * The intent behind a connection buffer is to facilitate reading and writing
 * from and to connections by taking care of the leftovers of invocations of
 * either the deserializer or `write()`.
 * This is _not_ implemented as a ring-buffer, since we're assuming relatively
 * small payloads, which may be processed once at a time.
 * Instead, the idea is to have one buffer, which is logically divided into
 * three areas:
 *  * data which may be written to a destination or deserialized
 *  * reserved space, to which a `read()` or a serializer may write to
 *  * free space, which is currently not used
 *
 * While each of these areas may have a size of 0, they are cohesive,
 * interleaving data is not possible.
 *
 *     +--------------+----------------+------------+
 *     | written data | reserved space | free space |
 *     +--------------+----------------+------------+
 *     |                                            |
 *     |<------------ total buffer size ----------->|
 *
 * Upon initialization, a buffer is initialized with an initial size.
 * All of the buffer is considered free space at this point.
 *
 * Writing entities may now reserve memory, which is subtracted from the free
 * space.
 * The reserved space will be passed to the caller, which may now start writing
 * to that memory.
 * Once the writing entity is done writing, it _must_ call `ws_connbuf_append()`
 * to communicate to the `ws_connbuf` that it finalized the write.
 * The amount of bytes passed to that call will be added to the "written data",
 * while the "reserved space" will be reset to `0`.
 *
 * Reading entities may deserialize or `write()` data from the "written data"
 * portion.
 * After doing so, it should `ws_connbuf_discard()`, which will remove the
 * given amount of "written data" by _moving_ the rest of the "written data" and
 * all of the "reserved space" towards the beginning of the buffer.
 *
 * @memberof ws_connbuf
 */
struct ws_connbuf {
    char* buffer; //!< Pointer to the allocated address
    size_t size; //!< Size of the allocated memory
    size_t data; //!< Amount of the used memory
    bool blocked; //!< Switch, to block discard actions directly after reserving
};

/**
 * Initializes the buffer on a given ws_connbuf struct
 *
 * @memberof ws_connbuf
 *
 * @return 0 in case of success, else negative error value from errno.h
 */
int
ws_connbuf_init(
    struct ws_connbuf* self, //!< The object
    size_t amount //!< The amount of bytes to be allocated
);

/**
 * Frees the buffer in a  given ws_connbuf struct
 *
 * @memberof ws_connbuf
 */
void
ws_connbuf_deinit(
    struct ws_connbuf* self //!< The address of the ws_connbuf struct
);

/**
 * Checks if `amount` bytes from a given buffer are available and if so, blocks
 * the buffer from beeing discarded
 *
 * @memberof ws_connbuf
 *
 * @return The address of the first free byte in the buffer on success,
 * else NULL
 */
char*
ws_connbuf_reserve(
    struct ws_connbuf* self, //!< The object
    size_t amount  //!< The amount how much memory will be reserved
);

/**
 * Get the number of bytes which may be reserved
 *
 * This method will return the number of bytes which may be reserves using
 * `ws_connbuf_reserve()`.
 *
 * @return the number of bytes available
 */
size_t
ws_connbuf_available(
    struct ws_connbuf* self //!< The object
);

/**
 * Increase the `data` member of the ws_connbuf struct and unblocks the buffer
 * from beeing discarded
 *
 * @memberof ws_connbuf
 *
 * @return 0 on success, else a negative value from errno.h
 */
int
ws_connbuf_append(
    struct ws_connbuf* self, //!< The object
    size_t amount //!< The amount how much the `data` member will be increased
);

/**
 * Releases a given amount of the buffer and move its content to the beginning
 *
 * @memberof ws_connbuf
 *
 * @return 0 on success, else a negative value from errno.h
 */
int
ws_connbuf_discard(
    struct ws_connbuf* self, //!< The object
    size_t amount //!< the amount of memory to release
);

#endif // __WS_CONNBUF_H__

/**
 * @}
 */
