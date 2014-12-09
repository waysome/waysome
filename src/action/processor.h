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

/**
 * @addtogroup action "Action manager"
 *
 * @{
 */

/**
 * @addtogroup action_processor "Action manager processor"
 *
 * @{
 */

#ifndef __WS_ACTION_PROCESSOR_H__
#define __WS_ACTION_PROCESSOR_H__

#include <malloc.h>
#include <stddef.h>

#include "util/attributes.h"

// forward declarations
struct ws_processor_stack;
struct ws_statement;
struct ws_transaction_command_list;

/**
 * Command processor context
 *
 * Structs of this type hold the state of a command processor.
 */
struct ws_processor {
    struct ws_processor_stack* stack; //!< @public the stack buffer
    struct ws_transaction_command_list* commands; //!< @public cmds to process
    struct ws_statement* pc; //!< @private current statement
    size_t stack_frame; //!< @private stack frame of the processor
};

/**
 * Initialize a command processor
 */
int
ws_processor_init(
    struct ws_processor* self, //!< command processor to initialize
    struct ws_processor_stack* stack, //!< stack to use
    struct ws_transaction_command_list* commands //!< commands to run
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Deinitialize a command processor
 */
void
ws_processor_deinit(
    struct ws_processor* self //!< command processor to initialize
)
__ws_nonnull__(1)
;

/**
 * Run a command processor with a context set up
 *
 * @return 0 on success, a negative error value on failure and a positive value
 *         if some command caused a jump outside the command list given by the
 *         transaction
 */
ssize_t
ws_processor_exec(
    struct ws_processor* self //!< command processor context
)
__ws_nonnull__(1)
;

/**
 * Perform a jump (by incrementing the program counter)
 *
 * This method performs a forward-jump, performing bounding checks.
 * If the forward-jump results in a position within the current scope of
 * statements held by the context, it performs the jump by incrementing the pc.
 * If, however, the jump would result in a positoion _outside_ the scope, the
 * jump is not performed but the function returns the position the jump would
 * result in.
 *
 * @return 0 if the jump was successful, a positive value if the jump would
 *           result in a position outside the current statement list.
 */
size_t
ws_processor_jump(
    struct ws_processor* self, //!< command processor context
    size_t value //!< value for incrementation of the PC
)
__ws_nonnull__(1)
;

#endif // __WS_ACTION_PROCESSOR_H__

/**
 * @}
 */

/**
 * @}
 */

