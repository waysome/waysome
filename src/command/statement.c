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

#include "command/statement.h"
#include "values/value.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Get a new `ws_argument` to write to
 *
 * @warning the memory is not nulled
 *
 * @return a new, uninitialized ws_argument or NULL
 */
static struct ws_argument*
command_args_append(
    struct ws_command_args* self //!< statement to append to
)
__ws_nonnull__(1)
;


/*
 *
 * Interface implementation
 *
 */

int
ws_statement_init(
    struct ws_statement* self
) {
    self->command = NULL;
    self->args.num = 0;
    self->args.vals = NULL;
    return 0;
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

/*
 *
 * Internal implementation
 *
 */

static struct ws_argument*
command_args_append(
    struct ws_command_args* self
) {
    if (!self->vals) {
        // allocate memory for the first argument
        self->vals = malloc(sizeof(*self->vals));
        if (self->vals) {
            self->num = 1;
        }
        return self->vals;
    }

    // check whether we have to resize by comparing the curren size to steps
    // where we have to resize
    size_t nsize = 1;
    while (nsize < self->num) {
        // we already have superseeded this step threshold
        if (nsize > self->num) {
            // the next step size is "far" away. We're fine
            self->num++;
            return self->vals;
        }

        // check the next stepping theshold
        nsize *= 2;
    }

    // nsize == self->num now. This means we're _on_ a stepping threshold

    // double the size of the memory area
    nsize *= 2;
    struct ws_argument* newargs;
    newargs = realloc(self->vals, sizeof(*self->vals) * nsize);
    if (!newargs) {
        return NULL;
    }

    self->num++;
    self->vals = newargs;
    return newargs;
}

