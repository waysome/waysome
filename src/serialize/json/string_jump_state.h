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
 * @addtogroup serializer_module_json_backend_deser "JSON backend deserializer"
 *
 * Serializer module JSON backend private utilities
 *
 * @{
 */

#ifndef __WS_SERIALIZE_JSON_DESERIALIZER_STRING_JUMP_TAB_COMMON_H__
#define __WS_SERIALIZE_JSON_DESERIALIZER_STRING_JUMP_TAB_COMMON_H__

#include "serialize/json/states.h"

/**
 * Get the next state for the current state and a string
 */
enum json_backend_state
get_next_state_for_string(
    enum json_backend_state current,
    const unsigned char * str
);

#endif //__WS_SERIALIZE_JSON_DESERIALIZER_STRING_JUMP_TAB_COMMON_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
