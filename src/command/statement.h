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

#ifndef __WS_COMMAND_STATEMENT_H__
#define __WS_COMMAND_STATEMENT_H__

#include <malloc.h>
#include <stdbool.h>

#include "util/attributes.h"

// forward declarations
struct ws_command;


/**
 * A command argument
 *
 * Naturally arguments may be passed to commands.
 * This type represents an argument being passed to a command which may be
 * processed by a processor.
 *
 * In most cases, commands take values (@see ws_command), which may be passed
 * via a stack (@see stack_semantics) or directly.
 *
 * To differentiate between those two modes of argument passing, this type
 * contains an enumeration `type`, which may either be set to `direct` or
 * `indirect` denoting whether the argument is to be passed directly or
 * indirectly.
 *
 * Of course, each mode requires the argument to be present in a format
 * suitable for the mode.
 * While the argument is a `ws_value` for an argument passed directly, the
 * argument is given as an _offset_ if it's passed indirectly.
 * Hence, the argument is embedded in a union.
 *
 * The correct usage of this struct is imperative for both construction and
 * usage of instances of this type.
 * To facilitate construction, the functions `ws_statement_append_direct()`
 * and `ws_statement_append_direct()` should be used.
 */
struct ws_argument {
    enum {
        indirect, //!< argument is given by position
        direct //!< argument is given directly
    } type; //!< @public type of argument (direct or indirect)
    union {
        ssize_t pos; //!< position, negative ones are from the top of the stack
        struct ws_value* val; //!< a value
    } arg; //!< @public the argument
}; 

/**
 * Command arguments
 * 
 * This struct represents command arguments applied to a command as part of a
 * statement.
 *
 * @note If `vals` is `NULL`, it indicates that the `num` topmost elements which
 *       are currently on the stack are arguments which should be passed to the
 *       command.
 */
struct ws_command_args {
    size_t num; //!< @public number of arguments
    struct ws_argument* vals; //!< @public arguments
};

/**
 * Statement
 *
 * This type represents a statement which may be executed by the command
 * processor.
 * A statement is a command invocation, hence a statement only consists of a
 * command (or, to be more precise, of a _pointer_ to a command) and the
 * arguments passed to that very command.
 *
 * Statements should _only_ be constructed using `ws_statement_init()`, which
 * takes care of the `command` field.
 * After construction, arguments may be appended by calls to
 * `ws_statement_append_direct` or `ws_statement_append_indirect`.
 */
struct ws_statement {
    struct ws_command const* command; //!< @public command to invoke
    struct ws_command_args args; //!< @public arguments to invoke the command with
};

/**
 * Initialize a statement
 */
int
ws_statement_init(
    struct ws_statement* self, //!< statement to initialize
    char const* name //!< name of the command to initialize the statement with
)
__ws_nonnull__(1, 2)
;

/**
 * Add a direct argument to a statement
 *
 * @return 0 if the operation was successful, a negative error value otherwise
 */
int
ws_statement_append_direct(
    struct ws_statement* self, //!< statement to append argument to
    struct ws_value* val //!< value to append
)
__ws_nonnull__(1, 2)
;

/**
 * Add a indirect argument to a statement
 *
 * @return 0 if the operation was successful, a negative error value otherwise
 */
int
ws_statement_append_indirect(
    struct ws_statement* self, //!< statement to append argument to
    ssize_t pos //!< position of indirect argument
)
__ws_nonnull__(1)
;

/**
 * Deinitialize a statement
 *
 * @return true if deinitialization was successfull, else false
 */
bool
ws_statement_deinit(
    struct ws_statement* self
);

#endif // __WS_COMMAND_STATEMENT_H__

