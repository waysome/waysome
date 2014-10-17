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

#include "command/command.h"
#include "objects/message/message.h"
#include "objects/message/transaction.h"

struct ws_transaction*
ws_transaction_new(
    size_t id,
    struct ws_string* name,
    enum ws_transaction_flags flags,
    struct ws_transaction_command_list* cmds
) {
    struct ws_transaction* t = calloc(1, sizeof(*t));

    if (!t) {
        return NULL;
    }

    if (!ws_message_init(&t->m, id)) {
        free(t);
        return NULL;
    }

    t->cmds = cmds;
    t->name = getref(name);
    t->flags = flags;

    return t;
}

uintmax_t
ws_transaction_connection_id(
    struct ws_transaction* t
) {
    uintmax_t id;

    ws_object_lock_read(&t->m.obj);
    id = t->connection_id;
    ws_object_unlock(&t->m.obj);

    return id;
}

void
ws_transaction_set_connection_id(
    struct ws_transaction* t,
    uintmax_t connection_id
) {
    ws_object_lock_write(&t->m.obj);
    t->connection_id = connection_id;
    ws_object_unlock(&t->m.obj);
}

enum ws_transaction_flags
ws_transaction_action(
    struct ws_transaction* t
) {
    return t->flags;
}

struct ws_string*
ws_transaction_name(
    struct ws_transaction* t
) {
    return getref(t->name);
}

struct ws_transaction_command_list*
ws_transaction_commands(
    struct ws_transaction* t
) {
    //!< @todo implement
    return NULL;
}
