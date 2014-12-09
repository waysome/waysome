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

/**
 * @addtogroup input "Input"
 *
 * @{
 */

#ifndef __WS_INPUT_UTILS_H__
#define __WS_INPUT_UTILS_H__

#include <stdbool.h>

#include "objects/set.h"

#define INPUT_DEVICE_NAME "event"

extern struct ws_input_context {
    struct ws_set devices;
} ws_input_ctx;

/**
 * Checks if the file is a file suitable for inclusion as a device file
 *
 * @returns true if it is suitable, false if not
 */
bool
ws_input_filter_event_device_name(
    const char* name
);

#endif // __WS_INPUT_UTILS_H__

/**
 * @}
 */

