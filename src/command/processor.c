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

#include <malloc.h>
#include <unistd.h>

#include "command/processor.h"
#include "connection/connector.h"
#include "objects/object.h"
#include "serialize/deserializer.h"
#include "serialize/serializer.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialize a command processor
 */
bool
command_processor_deinit(
    struct ws_object * obj
);


/*
 *
 * Interface implementation
 *
 */

struct ws_command_processor {
    struct ws_object obj; //!< @public parent object type
    struct ws_connector conn; //!< @public connection to process
    struct ws_deserializer* deserializer; //!< @public deserializer to use
    struct ws_serializer* serializer; //!< @public serializer to use
    bool is_init; //!< @public flag indicating whether it's initialized
};

/**
 * Type information for ws_wayland_obj type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_COMMAND_PROCESSOR = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_command_processor",

    .hash_callback = NULL,
    .init_callback = NULL,
    .deinit_callback = command_processor_deinit,
    .dump_callback = NULL,
    .run_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,
};

/*
 *
 * Interface implementations
 *
 */

struct ws_command_processor*
ws_command_processor_new(
    int fd, //!< file descriptor to use run the command processor on
    struct ws_deserializer* deserializer, //!< deserializer to use
    struct ws_serializer* serializer //!< serializer to use
) {
    // check whether the fd is sane
    if (fd < 0) {
        return NULL;
    }

    // allocate memory for the ws_command_processor
    struct ws_command_processor* retval = calloc(1, sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    // initialize the underlying object
    if (!ws_object_init(&retval->obj)) {
        goto cleanup_mem;
    }
    retval->obj.id = &WS_OBJECT_TYPE_ID_COMMAND_PROCESSOR;
    retval->obj.settings |= WS_OBJECT_HEAPALLOCED;

    int res;

    // check whether we have a readonly connection
    if (serializer) {
        // no
        res = ws_connector_init(&retval->conn, fd);
    } else {
        // yes
        res = ws_connector_init_readonly(&retval->conn, fd);
    }
    if (res < 0) {
        goto cleanup_mem;
    }

    // initialize the (de)serializer
    retval->deserializer    = deserializer;
    retval->serializer      = serializer;

    //!< @todo initialize and start a handler

    // mark the object as initialized
    retval->is_init = true;

    // return the command processor
    return retval;

    // cleanups

cleanup_mem:
    ws_object_deinit(&retval->obj);
    free(retval);

    return NULL;
}


/*
 *
 * Internal implementation
 *
 */

bool
command_processor_deinit(
    struct ws_object * obj
) {
    struct ws_command_processor* proc = (struct ws_command_processor*) obj;

    if (!proc->is_init) {
        return true;
    }

    proc->is_init = false;

    ws_connector_deinit(&proc->conn);

    ws_deserializer_deinit(proc->deserializer);
    if (proc->serializer) {
        ws_serializer_deinit(proc->serializer);
    }
    return true;
}

