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
#include "command/control.h"
#include "values/int.h"


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
            val = ws_processor_stack_value_at(proc->stack, arg->arg.pos);
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

