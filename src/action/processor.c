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

#include "action/processor.h"
#include "action/processor_stack.h"
#include "objects/message/transaction.h"

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
    //!< @todo implement
    return -1;
}

size_t
ws_processor_jump(
    struct ws_processor* self,
    size_t value
) {
    //!< @todo implement
    return 0;
}
