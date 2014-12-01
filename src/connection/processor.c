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
#include <ev.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#include "action/manager.h"
#include "connection/connector.h"
#include "connection/processor.h"
#include "objects/object.h"
#include "serialize/deserializer.h"
#include "serialize/serializer.h"

/*
 *
 * Forward declarations
 *
 */

/*
 * Dispatching watcher callback
 *
 * This callback processes input
 */
static void
connection_manager_dispatch(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_io* watcher, //!< watcher which triggered the update
    int revents //!< events
);

/**
 * Flushing watcher callback
 *
 * This callback flushes the connection
 */
static void
connection_manager_flush(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_prepare* watcher, //!< watcher which triggered the update
    int revents //!< events
);

/**
 * Try to flush the message currently queued
 *
 * @return 0 if the serializer may now take a new message, a negative error
 *         code on failure, especially `-EAGAIN` if the message was not
 *         flushed.
 */
static int
connection_manager_flush_msg(
    struct ws_connection_manager* proc,
    struct ws_message* message
);

/**
 * Deinitialize a command processor
 */
bool
connection_manager_deinit(
    struct ws_object * obj
);


/*
 *
 * Interface implementation
 *
 */

/**
 * @extends ws_object
 */
struct ws_connection_manager {
    struct ws_object obj; //!< @public parent object type
    struct ws_connector conn; //!< @public connection to process
    struct ws_deserializer* deserializer; //!< @public deserializer to use
    struct ws_serializer* serializer; //!< @public serializer to use
    ev_io dispatcher; //!< @public dispatching watcher
    ev_prepare flusher; //!< @public flushing watcher
    bool is_init; //!< @public flag indicating whether it's initialized
};

/**
 * Type information for ws_wayland_obj type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_COMMAND_PROCESSOR = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_connection_manager",

    .hash_callback = NULL,
    .deinit_callback = connection_manager_deinit,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};

/*
 *
 * Interface implementations
 *
 */

struct ws_connection_manager*
ws_connection_manager_new(
    int fd, //!< file descriptor to use run the command processor on
    struct ws_deserializer* deserializer, //!< deserializer to use
    struct ws_serializer* serializer //!< serializer to use
) {
    // check whether the fd is sane
    if (fd < 0) {
        return NULL;
    }

    // allocate memory for the ws_connection_manager
    struct ws_connection_manager* retval = calloc(1, sizeof(*retval));
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

    // now get the libev loop
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (!loop) {
        goto cleanup_mem;
    }

    if (!getref(retval)) {
        goto cleanup_mem;
    }

    // initialize watchers
    ev_io_init(&retval->dispatcher, connection_manager_dispatch, fd, EV_READ);
    retval->dispatcher.data = retval;
    ev_io_start(loop, &retval->dispatcher);

    if (serializer) {
        ev_prepare_init(&retval->flusher, connection_manager_flush);
        retval->flusher.data    = retval;
        ev_prepare_start(loop, &retval->flusher);
    }

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

static void
connection_manager_dispatch(
    struct ev_loop* loop,
    ev_io* watcher,
    int revents
) {
    struct ws_connection_manager* proc;
    proc = (struct ws_connection_manager*) watcher->data;
    ssize_t res;

    if (ws_object_lock_try_write(&proc->obj) != 0) {
        return;
    }

    // if we intend to reply to the messages, we should first check this
    if (proc->serializer) {
        res = connection_manager_flush_msg(proc, NULL);
        if ((res < 0) && (res != -EAGAIN) && (res != -EINTR)) {
            goto deinit;
        }
    }

    // try to read from the connection
    res = ws_connector_read(&proc->conn);
    if (res < 0) {
        goto error_handling;
    }

    struct ws_message* msg = NULL;
    while (1) {
        // deserialize a message
        //!< @todo use getters as soon as they are available
        res =  ws_deserialize(proc->deserializer, &msg,
                              proc->conn.inbuf.buffer, proc->conn.inbuf.data);
        if (res < 0) {
            break;
        }
        res = ws_connbuf_discard(&proc->conn.inbuf, res);
        if (res < 0) {
            break;
        }

        // handle the message
        if (!msg) {
            // nothing to do!
            ws_object_unlock(&proc->obj);
            return;
        }

        // pass the message to the transaction manager
        struct ws_reply* reply = ws_action_manager_process(msg);
        ws_object_unref((struct ws_object*) msg);
        if (!reply) {
            // reply being `NULL` can have a number of reasons
            continue;
        }

        // check whether we _can_ send a reply
        if (!proc->serializer) {
            // nope, we can't
            ws_object_unref((struct ws_object*) reply);
            continue;
        }

        // flush the buffer
        res = connection_manager_flush_msg(proc, (struct ws_message*) reply);
        if (res < 0) {
            break;
        }
    }

error_handling:
    switch(-res) {
    case -EAGAIN:
    case -EINTR:
        // we have to come back later
        ws_object_unlock(&proc->obj);
        return;

    default:
        //!< @todo report an error

    case EOF:
        // we reached the end of file
        ;
    }

deinit:
    connection_manager_deinit(&proc->obj);
    ws_object_unlock(&proc->obj);
    ws_object_unref(&proc->obj);
}

static void
connection_manager_flush(
    struct ev_loop* loop,
    ev_prepare* watcher,
    int revents
) {
    struct ws_connection_manager* proc;
    proc = (struct ws_connection_manager*) watcher->data;

    // try to lock the object
    if (ws_object_lock_try_write(&proc->obj) != 0) {
        goto unlock;
    }

    // flush the buffer
    int res = connection_manager_flush_msg(proc, NULL);
    if ((res == 0) || (res == -EAGAIN) || (res == -EINTR)) {
        goto unlock;
    }

    //!< @todo: error handling
    connection_manager_deinit(&proc->obj);
    ws_object_unlock(&proc->obj);
    ws_object_unref(&proc->obj);
    return;

unlock:
    ws_object_unlock(&proc->obj);
}

static int
connection_manager_flush_msg(
    struct ws_connection_manager* proc,
    struct ws_message* message
) {
    int res;
    // iterate until there's nothing left to do
    do {
        // allocate memory to write the reply
        size_t avail = ws_connbuf_available(&proc->conn.outbuf);
        char* buf = ws_connbuf_reserve(&proc->conn.outbuf, avail);
        if (!buf) {
            return -EAGAIN;
        }

        // serialize reply
        res = ws_serialize(proc->serializer, buf, avail, message);
        if (res < 0) {
            break;
        }

        // communicate the changes to the buffer
        res = ws_connbuf_append(&proc->conn.outbuf, res);
        if (res < 0) {
            break;
        }

        // flush
        res = ws_connector_flush(&proc->conn);
    } while (res == 0);
    return res;
}

bool
connection_manager_deinit(
    struct ws_object * obj
) {
    struct ws_connection_manager* proc = (struct ws_connection_manager*) obj;

    if (!proc->is_init) {
        return true;
    }

    proc->is_init = false;

    ws_connector_deinit(&proc->conn);

    ws_deserializer_deinit(proc->deserializer);
    if (proc->serializer) {
        ws_serializer_deinit(proc->serializer);
    }

    // now get the libev loop
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (!loop) {
        return true;
    }

    ev_io_stop(loop, &proc->dispatcher);
    if (proc->serializer) {
        ev_prepare_start(loop, &proc->flusher);
    }

    return true;
}

