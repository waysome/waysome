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
#include "objects/message/transaction.h"
#include "values/union.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Prepare arguments for command execution
 *
 * This function prepares the stack for command execution by copying everything
 * to the right position.
 */
static int
ws_processor_prepare_args(
    struct ws_processor_stack* stack,
    struct ws_command_args const* const args
)
__ws_nonnull__(1, 2)
;

/**
 * Execute a regaular command function
 *
 * This function executes a regular command function.
 * This includes preparing and cleaning up the stack.
 */
static int
exec_regular(
    struct ws_processor_stack* stack,
    ws_regular_command_func func,
    struct ws_command_args const* const args
)
__ws_nonnull__(1, 2, 3)
;

/*
 *
 * Interface implementation
 *
 */

int
ws_processor_init(
    struct ws_processor* self,
    struct ws_processor_stack* stack,
    struct ws_transaction_command_list* commands
) {
    // initialize all the fields
    self->stack         = stack;
    self->commands      = commands;
    self->pc            = commands->statements;
    self->stack_frame   = ws_processor_stack_start_frame(stack);

    // nothing can go wrong here
    return 0;
}

void
ws_processor_deinit(
    struct ws_processor* self
) {
    // restore the old frame
    (void) ws_processor_stack_restore_frame(self->stack, self->stack_frame);
}

ssize_t
ws_processor_exec(
    struct ws_processor* self
) {
    // keep a sentinel around for faster comparisons
    struct ws_statement const* aend;
    aend = self->commands->statements + self->commands->n;

    while (self->pc < aend) {
        // get the current pc and increment it afterwards
        struct ws_statement const* cur = self->pc++;

        // execute command
        int res;
        switch(cur->command->command_type) {
        case regular:
            res = exec_regular(self->stack, cur->command->func.regular,
                               &cur->args);
            break;

        case special:
            res = cur->command->func.special(self, &cur->args);
            break;

        default:
            return -ENOTSUP;
        }

        if (res < 0) {
            return res;
        }
    }

    if (self->pc <= aend) {
        return 0;
    }
    return self->pc - self->commands->statements;
}

size_t
ws_processor_jump(
    struct ws_processor* self,
    size_t value
) {
    //!< @todo implement
    return 0;
}


/*
 *
 * Internal implementation
 *
 */

static int
ws_processor_prepare_args(
    struct ws_processor_stack* stack,
    struct ws_command_args const* const args
) {
    union ws_value_union* top = ws_processor_stack_top(stack);
    size_t argc = args->num;

    //!< @todo optimize for if arguments already are in the right position
    {
        int res = ws_processor_stack_push(stack, argc);
        if (res < 0) {
            return res;
        }
    }

    // iterate over all the arguments and assemble the stack
    while (argc--) {
        // get the argument
        struct ws_argument* cur_arg = args->vals + argc;

        // determine the source of the argument
        struct ws_value* src = NULL;
        switch (cur_arg->type) {
        case direct:
            src = cur_arg->arg.val;

        case indirect:
            src = ws_processor_stack_value_at(stack, cur_arg->arg.pos);
        }

        if (!src) {
            return -EINVAL;
        }
        int res = ws_value_union_init_from_val(top + argc, src);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}

static int
exec_regular(
    struct ws_processor_stack* stack,
    ws_regular_command_func func,
    struct ws_command_args const* const args
) {
    union ws_value_union* top = NULL;
    if (args->num <= 0) {
        return -EINVAL;
    }

    int res;

    // prepare the stack for command execution
    if (&args->vals) {
        // the parameters are given explicitely
        top = ws_processor_stack_top(stack);
        res = ws_processor_prepare_args(stack, args);
        if (res < 0) {
            return res;
        }
    } else {
        // the parameters are given implicitely
        top = (union ws_value_union*)
              ws_processor_stack_value_at(stack, -(args->num));
    }

    // we somehow ended up with a bad stack
    if (!top) {
        return -EINVAL;
    }

    res = func(top);
    if (res < 0) {
        return res;
    }

    // pop the values
    return ws_processor_stack_pop(stack, args->num - 1);
}

