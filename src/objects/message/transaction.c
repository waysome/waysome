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

#include "command/command.h"
#include "command/statement.h"
#include "objects/message/message.h"
#include "objects/message/transaction.h"

/*
 *
 * static function declarations
 *
 */

/**
 * Deinitialize callback for ws_transaction
 */
static bool
deinit_transaction(
    struct ws_object* self
);

/*
 *
 * variables
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_TRANSACTION = {
    .supertype  = &WS_OBJECT_TYPE_ID_MESSAGE,
    .typestr    = "ws_transaction",

    .deinit_callback = deinit_transaction,
    .dump_callback = NULL,
    .run_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,
};

/*
 *
 * Interface implementation
 *
 */

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

    if (0 != ws_message_init(&t->m, id)) {
        free(t);
        return NULL;
    }
    t->m.obj.id = &WS_OBJECT_TYPE_ID_TRANSACTION;
    t->m.obj.settings |= WS_OBJECT_HEAPALLOCED;

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
ws_transaction_flags(
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
    return t->cmds;
}

int
ws_transaction_push_statement(
    struct ws_transaction* t,
    struct ws_statement* statement
) {
    if (!t->cmds) {
        t->cmds = calloc(1, sizeof(*t->cmds));
        if (!t->cmds) {
            return -ENOMEM;
        }

        t->cmds->statements = NULL;
        t->cmds->len          = 1;
        t->cmds->num        = 0;
    }

    if (t->cmds->len + 1 >= t->cmds->num) {
        struct ws_statement* tmp;
        size_t newsize = (t->cmds->len * 2) * sizeof(*t->cmds->statements);

        tmp = realloc(t->cmds->statements, newsize);

        if (!tmp) {
            return -ENOMEM;
        }

        t->cmds->statements = tmp;
        t->cmds->len *= 2;
    }

    t->cmds->statements[t->cmds->num].command = statement->command;
    t->cmds->statements[t->cmds->num].args.num = statement->args.num;
    t->cmds->statements[t->cmds->num].args.vals = statement->args.vals;
    t->cmds->num++;

    return 0;
}

/*
 *
 * static function implementations
 *
 */

static bool
deinit_transaction(
    struct ws_object* self
) {
    struct ws_transaction* t = (struct ws_transaction*) self;

    ws_object_unref((struct ws_object*) t->name);

    while (--t->cmds->len) {
        ws_statement_deinit(&t->cmds->statements[t->cmds->len]);
    }

    return true;
}
