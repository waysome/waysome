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

#include "action/processor_stack.h"

int
ws_processor_stack_init(
    struct ws_processor_stack* self
) {
    //!< @todo implement
    return -1;
}

void
ws_processor_stack_deinit(
    struct ws_processor_stack* self
) {
    //!< @todo implement
}

int
ws_processor_stack_push(
    struct ws_processor_stack* self,
    size_t slots
) {
    //!< @todo implement
    return -1;
}

int
ws_processor_stack_pop(
    struct ws_processor_stack* self,
    size_t slots
) {
    //!< @todo implement
    return -1;
}

union ws_value_union*
ws_processor_stack_top(
    struct ws_processor_stack* self
) {
    //!< @todo implement
    return NULL;
}

union ws_value_union*
ws_processor_stack_bottom(
    struct ws_processor_stack* self
) {
    //!< @todo implement
    return NULL;
}

