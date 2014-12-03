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

#include <stdlib.h>
#include <string.h>

#include "connection/manager.h"
#include "objects/set.h"
#include "util/socket.h"

#define SOCK_NAME "waysome.sock"

/*
 *
 * Forward declarations
 *
 */

/**
 * Callback for creating a connection
 *
 * @return zero on success, else negative errno.h number
 */
int
create_connection_cb(
    int fd //!< File descriptor
);

/*
 *
 * Internal variables
 *
 */

static struct ws_connection_manager {
    struct ws_set connections; //!< @protected Connection processors set
    struct ws_socket sock; //!< @protected The socket
} connman;

/*
 *
 * Interface implementation
 *
 */

int
ws_connection_manager_init(void)
{
    static bool is_init = false;
    if (is_init) {
        return 0;
    }

    memset(&connman, 0, sizeof(connman));
    int res = ws_set_init(&connman.connections);
    if (res != 0) {
        return res;
    }

    res = ws_socket_init(&connman.sock, create_connection_cb, SOCK_NAME);
    if (res != 0) {
        ws_object_deinit(&connman.connections.obj);
        return res;
    }

    is_init = true;
    return 0;
}

/*
 *
 * Interface implementation
 *
 */

int
create_connection_cb(
    int fd
) {
    //!< @todo implement
    return -1;
}

