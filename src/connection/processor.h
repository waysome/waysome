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

#ifndef __WS_CONNECTION_PROCESSOR_H__
#define __WS_CONNECTION_PROCESSOR_H__

#include <ev.h>

#include "connection/connector.h"
#include "objects/object.h"
#include "util/attributes.h"

// forward declarations
struct ws_deserializer;
struct ws_serializer;

/**
 * @extends ws_object
 */
struct ws_connection_processor {
    struct ws_object obj; //!< @protected parent object type
    struct ws_connector conn; //!< @protected connection to process
    struct ws_deserializer* deserializer; //!< @protected deserializer to use
    struct ws_serializer* serializer; //!< @protected serializer to use
    ev_io dispatcher; //!< @protected dispatching watcher
    ev_prepare flusher; //!< @protected flushing watcher
    bool is_started; //!< @protected flag indicating whether it's started
};

/**
 * Variable which holds type information about the ws_connection_processor type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_CONNECTION_PROCESSOR;

/**
 * Create a connection processor for a connection (which it creates internally)
 *
 * This function creates and starts a connection processor for a given
 * connection. For deserialisation, a `deserializer` must be supplied.
 * However, the `serializer` is completely optional.
 * If _no_ serializer is passed, the connection will be a read-only connection.
 *
 * @return a new connection processor or `NULL`, if an error occured
 */
struct ws_connection_processor*
ws_connection_processor_new(
    int fd, //!< file descriptor to use run the connection processor on
    struct ws_deserializer* deserializer, //!< deserializer to use
    struct ws_serializer* serializer //!< serializer to use
)
__ws_nonnull__(2)
;

/**
 * Start a connection
 *
 * @return 0 on success, a negative error value otherwise
 */
int
ws_connection_processor_start(
    struct ws_connection_processor* conn //!< connection processor to start
)
__ws_nonnull__(1)
;

/**
 * Close a connection
 *
 * @note it's save to call this function multiple times
 *
 * @return 0 on success, a negative error value otherwise
 */
int
ws_connection_processor_close(
    struct ws_connection_processor* conn //!< connection processor to start
)
__ws_nonnull__(1)
;

#endif // __WS_CONNECTION_PROCESSOR_H__

/**
 * @}
 */

