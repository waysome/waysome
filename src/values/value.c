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

#include "values/value.h"
#include "values/value_type.h"

#include "util/condition.h"

void
ws_value_init(
    struct ws_value* self
) {
    if (self) {
        self->type = WS_VALUE_TYPE_VALUE;
        self->deinit_callback = ws_value_deinit;
    }
}

void
ws_value_deinit(
    struct ws_value* self
) {
    if (self) {
        if (self->deinit_callback &&
                likely(self->deinit_callback != ws_value_deinit)) {
            self->deinit_callback(self);
        }
    }
}

enum ws_value_type
ws_value_get_type(
    struct ws_value* self
) {
    return self->type;
}
