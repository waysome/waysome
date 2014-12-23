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
 * @addtogroup connection "Connection"
 *
 * @{
 */

#ifndef __WS_CONNECTION_MANAGER_H__
#define __WS_CONNECTION_MANAGER_H__

#include <stdbool.h>

// forward declarations
struct ws_connection_processor;

/**
 * Initialize the connection manager singleton
 *
 * @return zero on success, a negative errno.h number on failure
 */
int
ws_connection_manager_init(void);

/**
 * Manually open a connection, mostly usefull for the config loading
 *
 * @memberof ws_connection_manager
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_connection_manager_open_connection(
    int fd, //!< File descriptor
    bool ro //!< Flag: true for read-only connection
);

/**
 * Close a connection
 *
 * @memberof ws_connection_manager
 *
 * @note only for internal use
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_connection_manager_close_connection(
    struct ws_connection_processor* proc //!< connection to remove
);

#endif // __WS_CONNECTION_MANAGER_H__

/**
 * @}
 */

