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

#include <malloc.h>

#include "input/hotkey_event.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Deinitialize a hotkey event
 */
static bool
deinit_hotkey(
    struct ws_object* self //!< object to deintialize
)
__ws_nonnull__(1)
;


/*
 *
 * Internal constants
 *
 */

/**
 * Type information for ws_hotkey_event type
 */
ws_object_type_id WS_OBJECT_TYPE_ID_HOTKEY_EVENT = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_hotkey_event",

    .deinit_callback = deinit_hotkey,

    .dump_callback = NULL,
    .run_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};


/*
 *
 * Interface implementation
 *
 */

struct ws_hotkey_event*
ws_hotkey_event_new(
    struct ws_string* name,
    uint16_t* codes,
    uint16_t code_num
) {
    // check whether the codes are unique
    {
        uint16_t* code = codes + code_num;
        // for each code ...
        while (--code > codes) {
            // ...check for duplicates
            uint16_t* cur = code;
            while (cur-- > codes) {
                if (*code == *cur) {
                    // found one
                    return NULL;
                }
            }
        }
    }

    struct ws_hotkey_event* retval;
    retval = (struct ws_hotkey_event*) ws_object_new(sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    if (!ws_string_init(&retval->name)) {
        goto cleanup_object;
    }

    if (!ws_string_set_from_str(&retval->name, name)) {
        goto cleanup_string;
    }

    retval->codes = codes;
    retval->code_num = code_num;

    return retval;

cleanup_string:
    ws_object_deinit(&retval->name.obj);

cleanup_object:
    ws_object_deinit(&retval->obj);
    free(retval);
    return NULL;
}


/*
 *
 * Internal implementation
 *
 */

static bool
deinit_hotkey(
    struct ws_object* self
) {
    struct ws_hotkey_event* ev = (struct ws_hotkey_event*) self;

    ws_object_deinit(&ev->name.obj);
    ws_object_deinit(&ev->obj);
    return true;
}

