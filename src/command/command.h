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

#ifndef __WS_COMMAND_COMMAND_H__
#define __WS_COMMAND_COMMAND_H__

#include <malloc.h>

// forward declarations
struct ws_command_processor;
union ws_value_union;

/**
 * A command argument
 *
 * There are two types of arguments: the ones passed directly and the ones which
 * are already on the stack.
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
 */
struct ws_command_args {
    size_t num; //!< @public number of arguments
    struct ws_argument* vals; //!< @public arguments
};

/**
 * Function type for command implementation
 *
 * Functions of this type will implement commands which may be use in
 * transactions.
 */
typedef int (*ws_regular_command_func)(union ws_value_union*);

/**
 * Function type for special commands
 *
 * This callback type may be used for implementation of special commands, like
 * jumps.
 *
 */
typedef int (*ws_special_command_func)(struct ws_command_processor*,
                                       struct ws_command_args*);

/**
 * Command type
 *
 * This datatype represents a command, which may be invoked in a transaction.
 */
struct ws_command {
    char const* const name; //!< @public name of the command

    enum {
        regular, //!< it's a regular command
        special //!< it's a special command (like jump)
    } command_type; //!< @public type of the command

    union {
        ws_regular_command_func regular; //!< @public regular commands' callback
        ws_special_command_func special; //!< @public special commands' callback
    } func; //!< @public function callback
};

/**
 * Statement
 *
 * This datatype represents a statement, which is part of an transaction
 */
struct ws_statement {
    struct ws_command const* command; //!< @public command to invoke
    struct ws_command_args args; //!< @public arguments to invoke the command with
};


/**
 * Find a command by name
 *
 * @returns command with the name given or NULL
 */
struct ws_command*
ws_command_get(
    char const* name //!< name of the command
);

#endif // __WS_COMMAND_COMMAND_H__

