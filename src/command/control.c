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

#include "action/processor.h"
#include "action/processor_stack.h"
#include "command/command.h"
#include "command/control.h"
#include "command/statement.h"
#include "values/int.h"
#include "values/union.h"


int
ws_builtin_cmd_jump(
    struct ws_processor* proc,
    struct ws_command_args const* const args
) {
    // the jump value is either embedded in an arg or this number itself
    size_t value = args->num;

    if (!value) {
        // no (valid) arguments?
        return -1;
    }

    struct ws_argument* arg = args->vals;
    if (arg) {
        // the value to jump is given by some ws_value
        if (value != 1) {
            // wrong number of arguments
            return -EINVAL;
        }

        // get the value
        struct ws_value* val;
        switch(arg->type) {
        case indirect: // the value must be extracted from the stack
            val = ws_processor_stack_value_at(proc->stack, arg->arg.pos, NULL);
            break;

        case direct: // the value is passed directly
            val = arg->arg.val;
            break;

        default:
            return -EINVAL;
        }

        // check whether it's an int
        if (val->type != WS_VALUE_TYPE_INT) {
            // wrong type
            return -EINVAL;
        }

        // set the jump value
        value = ws_value_int_get((struct ws_value_int*) val);
    }

    return ws_processor_jump(proc, value);
}


int
ws_builtin_cmd_store(
    struct ws_processor* proc,
    struct ws_command_args const* const args
) {
    // We want 2 arguments: The object to store, the position on the stack
    if (args->num != 2) {
        return -EINVAL; // Wrong number of args
    }

    struct ws_argument* arg = args->vals;
    if (!arg) {
        return -EINVAL;
    }

    struct ws_value* overrider;
    switch (arg[0].type) {
    case indirect: // The value must be extracted from the stack
        overrider = ws_processor_stack_value_at(proc->stack,
                                                arg[0].arg.pos,
                                                NULL);
        break;

    case direct: // The value is passed directly
        overrider = arg[0].arg.val;
        break;

    default:
        return -EINVAL;
    }

    if (arg[1].type == direct) {
        return -EINVAL;
    }

    union ws_value_union* old;
    old = (union ws_value_union*) ws_processor_stack_value_at(proc->stack,
                                                              arg[1].arg.pos,
                                                              NULL);

    ws_value_deinit(&old->value);
    return ws_value_union_init_from_val(old, overrider);
}

