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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "command/util.h"
#include "compositor/cursor.h"
#include "compositor/keyboard.h"
#include "compositor/wayland/surface.h"
#include "context.h"
#include "input/hotkeys.h"
#include "objects/object.h"
#include "objects/string.h"
#include "util/arithmetical.h"
#include "util/exec.h"
#include "util/string.h"
#include "values/string.h"
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

/**
 *  Get cursor under surface, regardless if it is in focus
 */
static int
func_get_surface_under_cursor(
    union ws_value_union* stack
);

/**
 * Give the plugin a way to get the currently focused surface by the mouse
 */
static int
func_get_ms_focus(
    union ws_value_union* stack
);

/**
 * Give the plugin a way to get the currently focused surface by the keyboard
 */
static int
func_get_kb_focus(
    union ws_value_union* stack
);

/**
 *  Set cursor focus
 */
static int
func_set_ms_focus(
    union ws_value_union* stack
);

/**
 *  Set keyboard focus
 */
static int
func_set_kb_focus(
    union ws_value_union* stack
);

static const struct ws_object_function functions[] = {
    { .name = "exit", .func = func_exit },
    { .name = "log", .func = func_log },
    { .name = "exec", .func = func_exec },
    { .name = "add_hotkey_event", .func = add_hotkey_event },
    { .name = "remove_hotkey_event", .func = remove_hotkey_event },
    { .name = "surface_under_cursor", .func = func_get_surface_under_cursor },
    { .name = "get_mouse_focus", .func = func_get_ms_focus },
    { .name = "get_keyboard_focus", .func = func_get_kb_focus },
    { .name = "set_ms_focus", .func = func_set_ms_focus },
    { .name = "set_kb_focus", .func = func_set_kb_focus },
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
    struct ws_string* ctx_str = NULL;
    int lvl = 0;

    stack += 2;

    // Context prefix
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }
    ctx_str = ws_value_string_get(&stack[0].string);
    ++stack;

    // Log lvl
    if (ws_value_get_type(&stack[0].value) != WS_VALUE_TYPE_INT) {
        return -EINVAL;
    }
    lvl = ws_value_int_get(&stack[0].int_);
    ++stack;

    size_t n = 0;
    union ws_value_union* iter;
    ITERATE_ARGS(iter, stack) {
        ++n;
    }

    char*   ary[n + 1];
    ary[n] = NULL; // NULL terminator (Hasta la vista, baby!)

    size_t i = 0;
    ITERATE_ARGS(iter, stack) {
        ary[i] = ws_value_union_tostr(iter);
        ++i;
    }

    if (ctx_str && lvl) {
        struct ws_logger_context log_ctx = { .prefix = ws_string_raw(ctx_str) };
        ws_log_ary(&log_ctx, lvl, ary);
    }

    while (--i) {
        free(ary[i]);
    }

    return 0;
}

static int
func_exec(
    union ws_value_union* stack
) {
    stack += 2; // We don't care about object itself and command name

    if (ws_value_get_type(&stack->value) != WS_VALUE_TYPE_STRING) {
        return -EINVAL;
    }

    size_t n = 0;
    enum ws_value_type iter_type;

    // count number of arguments and do type checking meanwhile
    while ((iter_type = ws_value_get_type(&stack[n].value)) ==
            WS_VALUE_TYPE_STRING) {
        ++n;
    }

    // check whether there is a command (argument 0)
    if ((iter_type != WS_VALUE_TYPE_NONE) || !n) {
        return -EINVAL;
    }

    // allocate room for a NULL-terminated list of args
    const char* args[n + 1];
    args[n] = NULL;

    // copy over all the strings
    size_t i = n;
    while (i) {
        --i;
        struct ws_string* arg = ws_value_string_get(&stack[i].string);
        if (!arg) {
            goto cleanup;
        }
        args[i] = ws_string_raw(arg);
    }

    // perform the exec
    int res = ws_exec(args[0], (char**)args);

cleanup:
    // free all the strings we did set
    while (n > i) {
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
    ws_object_unref((struct ws_object*) str);
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

static int
func_get_surface_under_cursor(
    union ws_value_union* stack
) {
    struct ws_surface* surface;
    surface = ws_cursor_get_surface_under_cursor(ws_cursor_get());
    ws_value_object_id_set(&stack[0].object_id, (struct ws_object*) surface);
    return 0;
}

static int
func_get_ms_focus(
    union ws_value_union* stack
) {
    struct ws_surface* surface = ws_cursor_get()->active_surface;

    ws_value_object_id_set(&stack[0].object_id, (struct ws_object*) surface);

    return 0;
}

static int
func_get_kb_focus(
    union ws_value_union* stack
) {
    struct ws_surface* surface = ws_keyboard_get()->active_surface;

    ws_value_object_id_set(&stack[0].object_id, (struct ws_object*) surface);

    return 0;
}

static int
func_set_ms_focus(
    union ws_value_union* stack
) {
    stack += 2; // We only want the object after this

    struct ws_cursor* cursor = ws_cursor_get();

    if (ws_value_get_type(&stack->value) != WS_VALUE_TYPE_OBJECT_ID) {
        ws_cursor_set_active_surface(cursor, NULL);
        return 0; // This unsets the focus
    }

    struct ws_object* maybe_surface = ws_value_object_id_get(&stack->object_id);

    if (maybe_surface->id != &WS_OBJECT_TYPE_ID_SURFACE) {
        return -EINVAL;
    }

    struct ws_surface* surface = (struct ws_surface*) maybe_surface;
    ws_cursor_set_active_surface(cursor, surface);

    return 0;
}

static int
func_set_kb_focus(
    union ws_value_union* stack
) {
    stack += 2; // We only want the object after this

    struct ws_keyboard* keyboard = ws_keyboard_get();

    if (ws_value_get_type(&stack->value) != WS_VALUE_TYPE_OBJECT_ID) {
        ws_keyboard_set_active_surface(keyboard, NULL);
        return 0; // This unsets the focus
    }

    struct ws_object* maybe_surface = ws_value_object_id_get(&stack->object_id);

    if (maybe_surface->id != &WS_OBJECT_TYPE_ID_SURFACE) {
        return -EINVAL;
    }

    struct ws_surface* surface = (struct ws_surface*) maybe_surface;
    ws_keyboard_set_active_surface(keyboard, surface);

    return 0;
}

