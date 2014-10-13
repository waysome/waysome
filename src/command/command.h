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
struct ws_value;

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

#endif // __WS_COMMAND_COMMAND_H__

