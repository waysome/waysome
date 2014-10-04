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
#include <pthread.h>
#include <string.h>

#include "objects/object.h"
#include "objects/string.h"

#include "values/value.h"
#include "values/value_named.h"

void
ws_value_named_value_init(
    struct ws_value_named_value* self
) {
}

int
ws_value_named_set_name(
    struct ws_value_named_value* self,
    char * str //!< new name
) {
    return 0;
}

int
ws_value_named_set_value(
    struct ws_value_named_value* self,
    struct ws_value* val
) {
    return 0;
}

char const*
ws_value_named_get_name(
    struct ws_value_named_value* self
) {
    return NULL;
}

struct ws_value*
ws_value_named_get_value(
    struct ws_value_named_value* self
) {
    return NULL;
}

signed int
ws_value_named_cmp(
    struct ws_value_named_value* self,
    struct ws_value_named_value* other
) {
    return 0;
}
