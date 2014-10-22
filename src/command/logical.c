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

#include <errno.h>

#include "command/logical.h"

#include "values/union.h"
#include "values/utils.h"
#include "values/value.h"

int
ws_builtin_cmd_lnot(
    union ws_value_union* args
) {
    bool val = ws_value_is_true(&args[0].value);
    ws_value_deinit(&args[0].value);
    ws_value_bool_init(&args[0].bool_);
    ws_value_bool_set(&args[0].bool_, val);
    return 0;
}

int
ws_builtin_cmd_land(
    union ws_value_union* args

) {
    struct ws_value* base = &args->value;

    while (ws_value_get_type(&args->value) != WS_VALUE_TYPE_NONE) {
        if (ws_value_is_true(&args->value)) {
            args++;
            continue;
        }

        ws_value_deinit((struct ws_value*) base);
        ws_value_bool_init((struct ws_value_bool*) base);
        ws_value_bool_set((struct ws_value_bool*) base, false);
        return 0;
    }

    ws_value_deinit((struct ws_value*) base);
    ws_value_bool_init((struct ws_value_bool*) base);
    ws_value_bool_set((struct ws_value_bool*) base, true);
    return 0;
}

int
ws_builtin_cmd_lnand(
    union ws_value_union* args

) {
    //!< @todo implement
    return -1;
}

int
ws_builtin_cmd_lor(
    union ws_value_union* args

) {
    //!< @todo implement
    return -1;
}

int
ws_builtin_cmd_lnor(
    union ws_value_union* args

) {
    //!< @todo implement
    return -1;
}

int
ws_builtin_cmd_lxor(
    union ws_value_union* args

) {
    //!< @todo implement
    return -1;
}

