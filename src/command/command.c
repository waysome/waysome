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

#include <stdbool.h>
#include <string.h>

#include "command/command.h"
#include "command/list.h"

#include "values/value.h"

#define LINEAR_THRESHOLD (4)


/*
 *
 * Interface implementation
 *
 */

struct ws_command const*
ws_command_get(
    char const* name //!< name of the command
) {
    // initialize bounds
    size_t first = 0;
    size_t alast = ws_command_cnt;
    size_t cur;

    // perform a binary search
    while (first + LINEAR_THRESHOLD > alast) {
        cur = (first + alast) / 2;

        // compare the current name with what we want
        int cmp = strcmp(name, ws_command_list[cur].name);

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
        return ws_command_list + cur;
    }

    // we are now in a realm where linear search is expected to be faster
    cur = alast;
    while (cur-- > first) {
        if (strcmp(name, ws_command_list[cur].name) == 0) {
            return ws_command_list + cur;
        }
    }

    // nothing found
    return NULL;
}

bool
ws_statement_deinit(
    struct ws_statement* self
) {
    while (self->args.num--) {
        if (self->args.vals[self->args.num].type == direct) {
            ws_value_deinit(self->args.vals[self->args.num].arg.val);
            free(self->args.vals[self->args.num].arg.val);
        }
    }

    free(self->args.vals);

    return true;
}

