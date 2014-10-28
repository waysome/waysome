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
 * @addtogroup serializer_module "Serializer module"
 *
 * @{
 */

/**
 * @addtogroup serializer_module_json_backend "Serializer JSON backend"
 *
 * @{
 */

/**
 * @addtogroup serializer_module_json_backend_common "JSON backend utils"
 *
 * Serializer module JSON backend private utilities
 *
 * @{
 */

#ifndef __WS_SERIALIZE_JSON_DESERIALIZER_STATES_H__
#define __WS_SERIALIZE_JSON_DESERIALIZER_STATES_H__

/**
 * For a full documentation about the states of the statemachine,
 * see @ref json_statemachine "the statemachine documentation".
 */

enum json_backend_state {
    STATE_INVALID, //!< Invalid state
    STATE_INIT, //!< Initial state, not valid besides when starting
    STATE_MSG, //!< "We are parsing the JSON object"
    STATE_UID, //!< "We parsed the "UID" key"
    STATE_COMMANDS, //!< We parsed the "commands" key
    STATE_TYPE, //!< We parsed the "type" key

    STATE_COMMAND_ARY, //!< We are parsing the command array
    STATE_COMMAND_ARY_NEW_COMMAND, //!< We are parsing a command
    STATE_COMMAND_ARY_COMMAND_NAME, //!< We parsed the command name
    STATE_COMMAND_ARY_COMMAND_ARGS, //!< We parse the command argument array

    STATE_COMMAND_ARY_COMMAND_ARG_DIRECT, //!< We are parsing a direct value

    STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT, //!< We parsed a indirect value
    STATE_COMMAND_ARY_COMMAND_ARG_INDIRECT_STACKPOS, //!< stack position arg

    STATE_STRING,
};

#endif //__WS_SERIALIZE_JSON_DESERIALIZER_STATES_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
