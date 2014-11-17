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

#include "objects/named.h"

/*
 *
 * Forward declarations
 *
 */

static bool
named_deinit(
    struct ws_object* obj
);


/*
 *
 * Internal constant
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_NAMED = {
    .supertype = &WS_OBJECT_TYPE_ID_STRING,
    .typestr = "ws_named",

    .deinit_callback = named_deinit,
    .dump_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .attribute_table = NULL,
};


/*
 *
 * Interface implementation
 *
 */

int
ws_named_init(
    struct ws_named* self,
    struct ws_string* name,
    struct ws_object* val
) {
    int res = ws_string_init(&self->str);
    if (res < 0) {
        return res;
        
    }
    self->str.obj.id = &WS_OBJECT_TYPE_ID_NAMED;
    self->str.obj.settings |= WS_OBJECT_HEAPALLOCED;

    if (!ws_string_set_from_str(&self->str, name)) {
        return -1;
    }

    if (val) {
        self->val = getref(val);
    }

    return 0;
}

struct ws_named*
ws_named_new(
    struct ws_string* name, //!< name of the object
    struct ws_object* val //!< valu eof the object
) {
    struct ws_named* retval = calloc(1, sizeof(*retval));
    if (!retval) {
        return NULL;
    }

    if (ws_named_init(retval, name, val) < 0) {
        free(retval);
        return NULL;
    }

    return retval;
}

struct ws_object*
ws_named_get_obj(
    struct ws_named* self
) {
    return getref(self->val);
}


/*
 *
 * Internal implementation
 *
 */

static bool
named_deinit(
    struct ws_object* obj
) {
    struct ws_named* self = (struct ws_named*) obj;

    if (self->val) {
        ws_object_unref(self->val);
    }

    return true;
}

