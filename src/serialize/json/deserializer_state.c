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

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <yajl/yajl_parse.h>
#include <wayland-util.h>

#include "command/command.h"
#include "serialize/json/common.h"
#include "serialize/json/deserializer_state.h"
#include "serialize/json/states.h"
#include "values/nil.h"
#include "values/bool.h"
#include "values/int.h"
#include "values/string.h"
#include "objects/string.h"

struct deserializer_state*
deserialize_state_new(
    yajl_callbacks* cbs,
    void* ctx
) {
    size_t s = sizeof(struct deserializer_state);
    struct deserializer_state* state;

    state = (struct deserializer_state*) serializer_yajl_state_new(s, cbs, ctx);
    if (!state) {
        return NULL;
    }

    state->current_state    = STATE_INIT;
    state->uid.uid          = NULL;
    state->tmp_command      = NULL;

    state->tmp_command_args.vals = NULL;
    state->tmp_command_args.num = 0;

    return state;
}

int
deserialize_state_init_cmds(
    struct deserializer_state* s,
    size_t cnt
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_new_arg(
    struct deserializer_state* s
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_mod_last_arg_set_direct(
    struct deserializer_state* s
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_mod_last_arg_set_indirect(
    struct deserializer_state* s,
    int val
) {
    //!< @todo implement
    return -1;
}

int
deserialize_state_mod_last_arg_set_null(
    struct deserializer_state* s
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_mod_last_arg_set_bool(
    struct deserializer_state* s,
    bool b
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_mod_last_arg_set_int(
    struct deserializer_state* s,
    int i
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_mod_last_arg_set_string(
    struct deserializer_state* s,
    struct ws_value_string* str
) {
    //!< @todo implement
    return 0;
}

int
deserialize_state_push_cmd(
    struct deserializer_state* s
) {
    //!< @todo implement
    return 0;
}
