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
#include <stdint.h>

#include "command/arithmetical.h"
#include "command/util.h"
#include "values/int.h"
#include "values/union.h"
#include "values/value.h"
#include "values/value_type.h"

int
ws_builtin_cmd_add(
    union ws_value_union* args
) {

    intmax_t sum = 0;
    intmax_t val;
    union ws_value_union* it;

    // iterate over all the arguments, checking whether they are ints
    ITERATE_ARGS_TYPE(it, args, val, int) {
        sum += val;
    }

    if (!AT_END(it)) {
        // Arrr! We must've hit something, captn!
        return -EINVAL;
    }

    ws_value_union_reinit(args, WS_VALUE_TYPE_INT);
    ws_value_int_set(&args->int_, sum);
    return 0;
}

int
ws_builtin_cmd_sub(
    union ws_value_union* args
) {
    //!< @todo: implement
    return 0;
}

int
ws_builtin_cmd_mul(
    union ws_value_union* args
) {
    //!< @todo: implement
    return 0;
}

int
ws_builtin_cmd_div(
    union ws_value_union* args
) {
    //!< @todo: implement
    return 0;
}
