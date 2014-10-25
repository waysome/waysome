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

#include "command/binary.h"
#include "command/util.h"
#include "values/value_type.h"
#include "values/union.h"

/*
 *
 * static function prototypes
 *
 */

/**
 * Do a binary not on the passed argument
 *
 * @warning This is a helper function for the ws_builtin_cmd_bnot() function
 * and should only be called after the argument array is validated. It doesn't
 * do any validation
 *
 * @return zero on success, else negative errno.h number
 */
int
bnot(
    union ws_value_union* args
);

/*
 *
 * interface implementation
 *
 */

int
ws_builtin_cmd_bnot(
    union ws_value_union* args
) {
    if (ws_value_get_type(&args->value) == WS_VALUE_TYPE_NONE ||
            ws_value_get_type(&args[1].value) != WS_VALUE_TYPE_NONE) {
        // There is no argument or there are more arguments than one
        return -EINVAL;
    }

    return bnot(args);
}

int
ws_builtin_cmd_band(
    union ws_value_union* args

) {
    intmax_t res = ~0;
    intmax_t val;
    union ws_value_union* it;

    ITERATE_ARGS_TYPE(it, args, val, int) {
        res &= val;
    }

    if (!AT_END(it)) {
        return -EINVAL;
    }

    ws_value_union_reinit(args, WS_VALUE_TYPE_INT);
    ws_value_int_set(&args->int_, res);
    return 0;
}

int
ws_builtin_cmd_bnand(
    union ws_value_union* args

) {
    int ret = ws_builtin_cmd_band(args);
    if (ret != 0) {
        return ret;
    }
    return bnot(args);
}

int
ws_builtin_cmd_bor(
    union ws_value_union* args

) {
    //!< @todo implement
    return 0;
}

int
ws_builtin_cmd_bnor(
    union ws_value_union* args

) {
    //!< @todo implement
    return 0;
}

int
ws_builtin_cmd_bxor(
    union ws_value_union* args

) {
    //!< @todo implement
    return 0;
}

/*
 *
 * static function implementations
 *
 */

int
bnot(
    union ws_value_union* args
) {
    if (ws_value_get_type(&args->value) == WS_VALUE_TYPE_INT) {
        int buf = ws_value_int_get(&args->int_);
        ws_value_deinit(&args->value);
        ws_value_int_init(&args->int_);
        ws_value_int_set(&args->int_, ~buf);
    } else {
        return -EINVAL;
    }

    return 0;
}
