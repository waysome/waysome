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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "connection/manager.h"
#include "connection/processor.h"
#include "objects/set.h"
#include "serialize/deserializer.h"
#include "serialize/json/deserializer.h"
#include "serialize/json/serializer.h"
#include "serialize/serializer.h"
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
    int res = 0;
    struct ws_serializer* ser = ws_serializer_json_serializer_new();
    if (!ser) {
        goto out;
    }

    struct ws_deserializer* deser = ws_serializer_json_deserializer_new();
    if (!deser) {
        res = -ENOMEM;
        goto clean_ser;
    }

    struct ws_connection_processor* p;
    p = ws_connection_processor_new(fd, deser, ser);
    if (!p) {
        res = -ENOMEM;
        goto clean_deser;
    }

    if (!ws_set_insert(&connman.connections, &p->obj)) {
        goto clean_deser;
    }

    goto out;
clean_deser:
    ws_deserializer_deinit(deser);

clean_ser:
    ws_serializer_deinit(ser);

out:
    return res;
}

