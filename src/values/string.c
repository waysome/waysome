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

#include <stdlib.h>

#include "values/string.h"

#include "objects/string.h"
#include "values/value.h"

/*
 *
 * forward delarations
 *
 */
static void
value_string_deinit(
    struct ws_value* self
)
__ws_nonnull__(1)
;

void
ws_value_string_init(
    struct ws_value_string* self
){
    if (self) {
        ws_value_init(&self->val);

        self->val.type = WS_VALUE_TYPE_STRING;
        self->val.deinit_callback = value_string_deinit;

        struct ws_string* str = ws_string_new();

        if (str) {
            str->is_utf8 = true;
            self->str = str;
        }
    }
}

struct ws_value_string*
ws_value_string_new(void)
{
    struct ws_value_string* wvs = calloc(1, sizeof(*wvs));

    if (wvs) {
        ws_value_string_init(wvs);
    }
      
    return wvs;
}

struct ws_string*
ws_value_string_get(
    struct ws_value_string* self
){
    if (self) {
        return self->str;
    }
    
    return NULL;
}

void
ws_value_string_set_str(
    struct ws_value_string* self,
    struct ws_string* str
) {
    if (self && str) {
        struct ws_string* new_str = getref(str);

        if (new_str) {
            ws_object_unref(&self->str->obj);           
            self->str = new_str;
        }
    }
}

/*
 *
 * static function implementations
 *
 */
static void
value_string_deinit(
    struct ws_value* self
) {
    struct ws_value_string* wvs = (struct ws_value_string*) self;
    
    if (!wvs) {
        return;
    }

    ws_object_unref(&wvs->str->obj);
}
