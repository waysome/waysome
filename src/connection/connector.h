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

#ifndef __WS_CONNECTOR_H__
#define __WS_CONNECTOR_H__

#include <stdbool.h>

#include "connection/connbuf.h"

/**
 * ws_connector type definition
 */

struct ws_connector {
    int fd; //!< @protected file descriptor
    struct ws_connbuf inbuf; //!< @public buffer for read data
    struct ws_connbuf outbuf; /**!< @public buffer for data 
                                * to be written in next flush*/
    bool readonly; //!< @protected Is the connector read-only?
};

/**
 * Initializes a ws_connector object
 *
 * @memberof ws_connector
 *
 * @return zero on success, else negative error code from errno.h
 */
int
ws_connector_init(
    struct ws_connector* self, //!< The connector object itself
    int fd //!< File descriptor
);

/**
 * Initializes a ws_connector object in read-only mode,
 * the output buffer is neither initialized nor used
 *
 * @memberof ws_connector
 *
 * @return zero on success, else negative error code form errno.h
 */
int
ws_connector_init_readonly(
    struct ws_connector* self, //!< The connector object itself
    int fd //!< File descriptor
);

/**
 * Deinitializes a ws_connector object
 *
 * @memberof ws_connector
 */
void
ws_connector_deinit(
    struct ws_connector* self //!< The connector object to deinitialize
);

/**
 * Read data from file descriptor, append to input buffer
 *
 * @memberof ws_connector
 *
 * @return zero on success, else negative error code from errno.h
 */
int
ws_connector_read(
    struct ws_connector* self
);

/**
 * Write data from output buffer which is then emptied
 *
 * @memberof ws_connector
 *
 * @return zero on succes, else -1 or negative error code from errno.h
 */
int
ws_connector_flush(
    struct ws_connector* self
);

#endif // __WS_CONNECTOR_H__
