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
#include <ev.h>
#include <stdint.h>

#include "context.h"
#include "command/util.h"
#include "input/hotkeys.h"
#include "objects/object.h"
#include "objects/string.h"
#include "values/string.h"
#include "values/union.h"
#include "util/exec.h"


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

/**
 * A way to start a programm
 */
static int
func_exec(
    union ws_value_union* stack
);

/*
 * Add hotkey event to context
 */
static int
add_hotkey_event(
    union ws_value_union* stack
);

/*
 * Remove hotkey event from context
 */
static int
remove_hotkey_event(
    union ws_value_union* stack
);

static const struct ws_object_function functions[] = {
    { .name = "exit", .func = func_exit },
    { .name = "log", .func = func_log },
    { .name = "exec", .func = func_exec },
    { .name = "add_hotkey_event", .func = add_hotkey_event },
    { .name = "remove_hotkey_event", .func = remove_hotkey_event },
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

static int
func_exec(
    union ws_value_union* stack
) {
    stack += 2; // We don't care about object itself and command name

    if (ws_value_get_type(&stack->value) != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }

    struct ws_string* strcmd    = ws_value_string_get(&stack->string);
    const char* cmd             = ws_string_raw(strcmd);

    stack++; // because `cmd` is extracted now

    size_t n;
    bool iter = true;
    enum ws_value_type iter_type;

    // count number of arguments and do type checking meanwhile
    for (n = 0; iter; n++) {
        iter_type = ws_value_get_type(&stack[n].value);
        if (iter_type != WS_VALUE_TYPE_STRING) {
            return -EINVAL;
        }

        iter = iter_type != WS_VALUE_TYPE_NONE;
    }

    const char* args[n];

    size_t i = n;
    while (i) {
        --i;
        struct ws_string* arg = ws_value_string_get(&stack[i].string);
        args[i] = ws_string_raw(arg);
    }

    int res = ws_exec(cmd, (char**)args);

    while (n) {
        --n;
        free((char*) args[n]);
    }

    return res;
}

static int
add_hotkey_event(
    union ws_value_union* stack
) {
    union ws_value_union* it;
    struct ws_string* str;
    
    union ws_value_union* retval = stack;
    stack += 2; //Ignoring object and command name

    if (ws_value_get_type(&stack->value) != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }

    str = ws_value_string_get(&stack->string);
    ++stack; // Ignore name, we already saved it

    int len = 0;
    intmax_t v;
    ITERATE_ARGS_TYPE(it, stack, v, int) {
        if (v > UINT16_MAX) {
            return -EINVAL;
        }
        ++len;
    }

    if (!AT_END(it)) {
        return -EINVAL;
    }

    size_t i = 0;
    uint16_t arr[len];
    ITERATE_ARGS_TYPE(it, stack, v, int) {
        arr[i] = (uint16_t) v;
        ++i;
    }

    int res = ws_hotkey_add(str, arr, len);
    ws_value_union_reinit(retval, WS_VALUE_TYPE_BOOL);
    ws_value_bool_set(&retval->bool_, res == 0);

    return res;
}

static int
remove_hotkey_event(
    union ws_value_union* stack
) {
    union ws_value_union* it;
    union ws_value_union* retval = stack;
    
    stack += 2; //ignoring object and command name

    int len = 0;
    intmax_t v;
    ITERATE_ARGS_TYPE(it, stack, v, int) {
        if (v > UINT16_MAX) {
            return -EINVAL;
        }
        ++len;
    }

    if (!AT_END(it)) {
        return -EINVAL;
    }

    size_t i = 0;
    uint16_t arr[len];
    ITERATE_ARGS_TYPE(it, stack, v, int) {
        arr[i] = (uint16_t) v;
        ++i;
    }

    int res =  ws_hotkey_remove(arr, len);
    ws_value_union_reinit(retval, WS_VALUE_TYPE_BOOL);
    ws_value_bool_set(&retval->bool_, res == 0);
 
    return res;
}
