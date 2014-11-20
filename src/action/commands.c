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
#include <string.h>

#include "action/commands.h"
#include "action/processor.h"
#include "action/processor_stack.h"
#include "command/command.h"
#include "command/statement.h"
#include "util/arithmetical.h"
#include "values/int.h"
#include "values/union.h"

/*
 *
 * Forward declarations
 *
 */

int
cmd_jump(
    struct ws_processor* proc,
    struct ws_command_args const* const args
);

int
cmd_store(
    struct ws_processor* proc,
    struct ws_command_args const* const args
);

int
cmd_push(
    struct ws_processor* proc,
    struct ws_command_args const* const args
);

int
cmd_pop(
    struct ws_processor* proc,
    struct ws_command_args const* const args
);

/*
 *
 * Interface implementation
 *
 */
int
ws_action_commands_init(void) {
    // list of commands to add. These _must_ be sorted!
    struct ws_command cmds[] = {
        {.name = "jump",  .command_type = special, .func.special = cmd_jump  },
        {.name = "store", .command_type = special, .func.special = cmd_store },
        {.name = "push",  .command_type = special, .func.special = cmd_push  },
        {.name = "pop",   .command_type = special, .func.special = cmd_pop   },
    };

    return ws_command_add(cmds, ARYLEN(cmds));
}


/*
 *
 * Internal implementation
 *
 */

int
cmd_jump(
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
cmd_store(
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

int
cmd_pop(
    struct ws_processor* proc,
    struct ws_command_args const* const args
) {
    size_t n = args->num;

    if (!n) {
        n = 1; // If we have no arguments, we pop 1 value from the stack.
    }

    struct ws_argument* arg = args->vals;
    if (!arg) {
        goto out;
    }

    struct ws_value* val;

    if (arg->type == direct) { // passed as ws_value_int
        val = arg->arg.val;
    } else { // passed as stack position
        val = ws_processor_stack_value_at(proc->stack, arg->arg.pos, NULL);
    }

    if (!val) {
        // could not find how many values should be popped.
        return -EINVAL;
    }

    if (ws_value_get_type(val) != WS_VALUE_TYPE_INT) {
        return -EINVAL; // Wrong type
    }

    n = ws_value_int_get((struct ws_value_int*) val);

out:
    return ws_processor_stack_pop(proc->stack, n);
}

int
cmd_push(
    struct ws_processor* proc,
    struct ws_command_args const* const args
) {
    size_t n = args->num;

    if (!n) {
        // If we have nothing to push, we push NIL
        return ws_processor_stack_push(proc->stack, 1);
    }

    int res = ws_processor_stack_push(proc->stack, n);
    if (res != 0) {
        return res;
    }

    // Get the (old) top
    union ws_value_union* top   = ws_processor_stack_top(proc->stack) - n;
    struct ws_value* v          = NULL;

    if (args->vals) { // There are values to push
        // Each value from the `args` array can either be a real value passed to
        // the command or a position on the stack.

        for (size_t i = 0 ; i < n; i++) {
            if (args->vals[i].type == indirect) { // it is a position
                v = ws_processor_stack_value_at(proc->stack,
                                                args->vals[i].arg.pos,
                                                NULL);
            } else { // It is real!
                v = args->vals[i].arg.val;
            }

            if (!v) {
                return -EINVAL;
            }
            ws_value_union_init_from_val(&top[i], v);
        }
    }
    // else: There are no values to push, NIL got pushed by the
    // ws_processor_stack_push() function, that's all we want

    return 0;
}

