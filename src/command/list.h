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

#ifndef __WS_COMMAND_LIST_H__
#define __WS_COMMAND_LIST_H__

#include "command/command.h"

#define DECLARE_CMD_regular(name_) int ws_builtin_cmd_##name_(\
    union ws_value_union*\
)

#define DECLARE_CMD_special(name_) int ws_builtin_cmd_##name_(\
    struct ws_command_processor*,\
    struct ws_command_args*\
)

#define COMMAND(name_, type_) {\
    .name = #name_,\
    .command_type = type_,\
    .func.type_ = ws_builtin_cmd_##name_,\
},

/**
 * List holding all the commands in alphabetical order
 */
extern struct ws_command ws_command_list[];

#endif // __WS_COMMAND_LIST_H__

