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
ws_value_string_deinit(
    struct ws_value* self
)
__ws_nonnull__(1)
;

void
ws_value_string_init(
    struct ws_value_string* self
){
}

struct ws_value_string*
ws_value_string_new(void){
    return NULL;
}

struct ws_string*
ws_value_string_get_str(
    struct ws_value_string* self
){
    return NULL;
}

void
ws_value_string_set_str(
    struct ws_value_string* self,
    struct ws_string str
) {
}

/*
 *
 * static function implementations
 *
 */
static void
ws_value_string_deinit(
    struct ws_value* self
) {
}
