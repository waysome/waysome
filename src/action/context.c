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

#include <ev.h>

#include "action/context.h"
#include "objects/object.h"
#include "objects/string.h"
#include "values/union.h"


/**
 * A way to exit waysome, to be used by plugins
 */
static int
func_exit(
    union ws_value_union* stack
);

/**
 *  Logging used by plugins
 */
static int
func_log(
    union ws_value_union* stack
);

static const struct ws_object_function functions[] = {
    { .name = "exit", .func = func_exit },
    { .name = "log", .func = func_log },
    { .name = NULL, .func = NULL }
};

ws_object_type_id WS_OBJECT_TYPE_ID_CONTEXT = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_context",

    .hash_callback = NULL,
    .deinit_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .function_table = functions,
    .attribute_table = NULL
};

static int
func_exit(
    union ws_value_union* stack
) {
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    ev_unloop(loop, EVUNLOOP_ALL);
    return 0;
}

static int
func_log(
    union ws_value_union* stack
) {
    int n = 0;
    struct ws_string* ctx_str = NULL;
    struct ws_string* str = NULL;
    int lvl = 0;
    union ws_value_union* cur;
    // If the type is NIL we know it is the last value in the stack.
    // NIL has the value 0 in this context and will thus exit the loop
    for(cur = stack; cur->value.type; ++cur, ++n) {
        switch (n) {
        case 2:
            if (cur->value.type == WS_VALUE_TYPE_STRING) {
                ctx_str = ws_value_string_get(&cur->string);
            }
            break;
        case 3:
            if (cur->value.type == WS_VALUE_TYPE_INT) {
                lvl = ws_value_int_get(&cur->int_);
            }
            break;
        case 4:
            if (cur->value.type == WS_VALUE_TYPE_STRING) {
                str = ws_value_string_get(&cur->string);
            }
            break;

        }
    }

    if (ctx_str && str && lvl) {
        struct ws_logger_context log_ctx = { .prefix = ws_string_raw(ctx_str) };
        ws_log(&log_ctx, lvl, ws_string_raw(str));
    }

    return 1;
}
