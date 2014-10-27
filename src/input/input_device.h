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

#ifndef __WS_INPUT_DEVICE_H__
#define __WS_INPUT_DEVICE_H__

#include <ev.h>

#include "objects/object.h"

/**
 * Input Device Type
 *
 * Objects of this type represent a specific event file which map to event
 * devices
 *
 * @extends ws_object
 */
struct ws_input_device {
    struct ws_object obj; //!< @protected parent object
    struct libevdev* dev; //!< @protected the libevdev object
    struct ev_io watcher; //!< @protected the evio watcher
    int fd; //!< @protocted the associated file descriptor
};

/**
 * Variable which holds the type information about the ws_message type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_INPUT_DEVICE;

/**
 * Initialize a ws_message
 *
 * @note for use by the derived types
 *
 * @return 0 on success, or a negative error code
 */
struct ws_input_device*
ws_input_device_new(
    int fd //!< The filedescriptor to the event file
);

#endif //__WS_INPUT_DEVICE_H__


