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
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#include "command/command.h"
#include "command/list.h"

#include "util/cleaner.h"
#include "values/value.h"

#define LINEAR_THRESHOLD (4)

/**
 * Command list
 */
struct {
    struct ws_command* commands; //!< @public commands
    size_t num; //!< @public number of commands
} cmd_ctx;


/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialization
 */
void
deinit_command(void* dummy);

/*
 *
 * Interface implementation
 *
 */

int
ws_command_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }
    is_init = true;

    cmd_ctx.commands = NULL;
    cmd_ctx.num = 0;

    int res = ws_command_add(ws_command_list, ws_command_cnt);
    if (res < 0) {
        return res;
    }

    return ws_cleaner_add(deinit_command, NULL);
}

struct ws_command const*
ws_command_get(
    char const* name //!< name of the command
) {
    if (ws_command_init() < 0) {
        return NULL;
    }

    // initialize bounds
    size_t first = 0;
    size_t alast = cmd_ctx.num;
    size_t cur;

    // perform a binary search
    while (first + LINEAR_THRESHOLD > alast) {
        cur = (first + alast) / 2;

        // compare the current name with what we want
        int cmp = strcmp(name, cmd_ctx.commands[cur].name);

        // the name we want is smaller
        if (cmp < 0) {
            alast = cur;
            continue;
        }

        // the name we want is greater
        if (cmp > 0) {
            first = cur + 1;
            continue;
        }

        // bullseye
        return cmd_ctx.commands + cur;
    }

    // we are now in a realm where linear search is expected to be faster
    cur = alast;
    while (cur-- > first) {
        if (strcmp(name, cmd_ctx.commands[cur].name) == 0) {
            return cmd_ctx.commands + cur;
        }
    }

    // nothing found
    return NULL;
}

int
ws_command_add(
    struct ws_command* commands,
    size_t num
) {
    // make sure we have enoug memory
    size_t dest = cmd_ctx.num + num;
    struct ws_command* buf = realloc(cmd_ctx.commands, sizeof(*buf) * dest);
    if (!buf) {
        return -ENOMEM;
    }

    // save away everything for now
    size_t src = cmd_ctx.num;
    cmd_ctx.commands = buf;
    cmd_ctx.num = dest;

    // merge the new commands into the list
    while (num && src) {
        --dest;
        // check what to insert here
        if (strcmp(commands[num-1].name, cmd_ctx.commands[src-1].name) > 0) {
            cmd_ctx.commands[dest] = commands[--num];
        } else {
            cmd_ctx.commands[dest] = cmd_ctx.commands[--src];
        }
    }

    // merge the last bit
    while (num--) {
        cmd_ctx.commands[num] = commands[num];
    }

    return 0;
}


/*
 *
 * Internal implementation
 *
 */

void
deinit_command(
    void* dummy
) {
    free(cmd_ctx.commands);
}

