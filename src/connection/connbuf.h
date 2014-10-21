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
 * ws_connbuf type definition
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
 * @return The address of the first free byte in the buffer on success, else NULL
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
