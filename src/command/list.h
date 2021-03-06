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
 * @addtogroup command "Command"
 *
 * @{
 */

/**
 * @addtogroup command_list "Command list"
 *
 * @{
 */

#ifndef __WS_COMMAND_LIST_H__
#define __WS_COMMAND_LIST_H__

#include "command/command.h"

/**
 * Regular command declaration macro
 *
 * This macro expands to the declaration of a regular command function for the
 * command `name_`.
 *
 * The macro is used in generated command function headers.
 */
#define DECLARE_CMD_regular(name_) int ws_builtin_cmd_##name_(\
    union ws_value_union*\
)

/**
 * Special command declaration macro
 *
 * This macro expands to the declaration of a special command function for the
 * command `name_`.
 *
 * The macro is used in generated command function headers.
 */
#define DECLARE_CMD_special(name_) int ws_builtin_cmd_##name_(\
    struct ws_processor*,\
    struct ws_command_args const* const\
)

/**
 * Command list entry
 *
 * This macro expands to a command list entry for a command which the name
 * `name_` and the type `type_`.
 * Valid types are `regular` and `special`.
 *
 * The macro is intended for use in the (generated) command list.
 */
#define COMMAND(name_, type_) {\
    .name = #name_,\
    .command_type = type_,\
    .func.type_ = ws_builtin_cmd_##name_,\
},

/**
 * List holding all the commands in alphabetical order
 *
 * The list is generated by the CMake setup from the entries in *.commands
 * files using the `COMMAND` macro.
 */
extern struct ws_command ws_command_list[];

/**
 * Number of commands in `ws_command_list`
 *
 * This constant is set by the same script which generates the
 * `ws_command_list`.
 */
extern size_t const ws_command_cnt;

#endif // __WS_COMMAND_LIST_H__

/**
 * @}
 */

/**
 * @}
 */

