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

#ifndef __WS_SERIALIZE_JSON_DESERIALIZER_STATE_H__
#define __WS_SERIALIZE_JSON_DESERIALIZER_STATE_H__

#include <stdbool.h>

#include "command/command.h"
#include "command/statement.h"

#include "serialize/json/common.h"
#include "serialize/json/states.h"

#include "values/string.h"

/**
 * Deserializer state object
 *
 * @extends serializer_yajl_state
 */
struct deserializer_state {
    struct serializer_yajl_state yajlstate; //!< @protected Base type.

    enum json_backend_state current_state; //!< @protected State identifier

    struct {
        const unsigned char* uid;
        size_t len;
    } uid; //!< @protected UID temporary storage

    enum {
        ALLOC_TYPE_TRANSACTION,
    } alloc_type; //!< @protected Allocation type flag

    struct wl_array* cmdbuf;

    struct ws_command* tmp_command;
    struct ws_command_args tmp_command_args;
};

/**
 * Allocate a new desertializer state object
 *
 * @return new object of `struct serializer_yajl_state_deserializer` or NULL
 */
struct deserializer_state*
deserialize_state_new(yajl_callbacks* cbs, void* ctx);

/**
 * Initialize the internal command buffer
 *
 * @memberof deserializer_state
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_init_cmds(
    struct deserializer_state* s,
    size_t cnt
);

/**
 * Add an new argument for the currently processed command object
 *
 * @memberof deserializer_state
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_new_arg(
    struct deserializer_state* s
);

/**
 * Make the last argument a direct one and set the position
 *
 * @memberof deserializer_state
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_direct(
    struct deserializer_state* s
);

/**
 * Make the last argument an indirect one
 *
 * @memberof deserializer_state
 *
 * @warning Does not allocate the ws_value
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_indirect(
    struct deserializer_state* s,
    int val
);

/**
 * Make the last argument an NULL one
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_null(
    struct deserializer_state* s //!< The current state object
);

/**
 * Make the last argument an boolean one
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_bool(
    struct deserializer_state* s, // The current state object
    bool b
);

/**
 * Make the last argument an integer one
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_int(
    struct deserializer_state* s, // The current state object
    int i
);

/**
 * Make the last argument an string one by setting it from the string value
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_mod_last_arg_set_string(
    struct deserializer_state* s, // The current state object
    struct ws_value_string* str // The string to set
);

/**
 * Add a new command object to the state
 *
 * @memberof deserializer_state
 *
 * Pushes the command from the `tmp_command` member to the command array buffer.
 * Also processes the stored arguments.
 *
 * @return zero on success, else negative errno.h number
 */
int
deserialize_state_push_cmd(
    struct deserializer_state* s
);

#endif //__WS_SERIALIZE_JSON_DESERIALIZER_STATE_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
