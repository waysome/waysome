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
 * @addtogroup objects "Classes"
 *
 * @{
 */

/**
 * @addtogroup objects_monitor "Class: Monitor type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_FRAMEBUFFER_DEVICE_H__
#define __WS_OBJECTS_FRAMEBUFFER_DEVICE_H__

#include <pthread.h>
#include <stdbool.h>

#include "objects/object.h"

/**
 * ws_framebuffer type definition
 *
 * @extends ws_object
 *
 * The ws_framebuffer
 */
struct ws_framebuffer_device {
    struct ws_object obj; //!< @protected Base class.
    int fd; //!< @public The filedescriptor
    char* path; //!< @public The path to the device file
};

/**
 * Variable which holds the type information about the ws_framebuffer_device
 * type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_FRAMEBUFFER_DEVICE;

/**
 * Get a Framebuffer Device provided by DRM using the given path
 *
 * This is the graphic card device that is given by the kernel. It loads it
 * and populates the given framebuffer with the information found.
 *
 * @memberof ws_framebuffer_device
 *
 * @return the device on success, NULL on failure
 */
struct ws_framebuffer_device*
ws_framebuffer_device_new(
    char* path //<! The path to the possible framebuffer device
);

#endif // __WS_OBJECTS_FRAMEBUFFER_DEVICE_H__

/**
 * @}
 */

/**
 * @}
 */
