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

#include <string.h>

#include "logger/module.h"
#include "input/utils.h"
#include "util/string.h"

struct ws_input_context ws_input_ctx;

bool
ws_input_filter_event_device_name(
    const char* name
) {
    if (ws_streq(".", name) || ws_streq("..", name)) {
        return false;
    }
    // strcmp also makes note of the length between the strings, we only want
    // it to check the beginning of the file name, in this case it has to match
    // "event*"
    if (!ws_strneq(INPUT_DEVICE_NAME, name, sizeof(INPUT_DEVICE_NAME) - 1)) {
        return false;
    }
    return true;
}
