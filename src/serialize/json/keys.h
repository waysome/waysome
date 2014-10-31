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

/**
 * @file keys.h
 *
 * This file contains JSON object key definitions. Each define here is one key
 * in the JSON object the deserializer wants to parse.
 */

#ifndef __WS_SERIALIZE_JSON_KEYS_H__
#define __WS_SERIALIZE_JSON_KEYS_H__

#define COMMANDS    "CMDS"
#define UID         "UID"
#define TYPE        "TYPE"
#define FLAGS       "FLAGS"

#define FLAG_EXEC   "EXEC"
#define FLAG_REGISTER "REGISTER"

#define TYPE_TRANSACTION "transaction"
#define TYPE_EVENT "event"

#define POS         "pos" // key for argument: stack position

#define EVENT_NAME  "name" // key for event name
#define EVENT_VALUE "value" // key for event value

#define EVENT       "event" // reply-message key for event reply
#define EVENT_CTX   "context" // "context" member of the event

#define EVENT_NAME  "name" // "name" member of the event

#endif //__WS_SERIALIZE_JSON_KEYS_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
