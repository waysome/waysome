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

#ifndef __WS_OBJECTS_MONITOR_H__
#define __WS_OBJECTS_MONITOR_H__

#include <stdbool.h>
#include <pthread.h>

#include "objects/object.h"
#include <xf86drmMode.h>

/**
 * ws_monitor type definition
 *
 * The ws_monitor
 */
struct ws_monitor {
    struct ws_object obj; //!< Supertype
    int connected; //!< is the monitor connected?
    uint32_t width; //!< width of the buffer
    uint32_t height; //!< height of the buffer
    uint32_t stride; //!< length of a line

    uint32_t size; //!< size of the map
    uint8_t* map; //!< mapped buffer data

    struct ws_framebuffer_device* fb_dev; //!< Framebuffer Device
    drmModeModeInfo mode; //!< mode of the monitor
    uint32_t handle; //!< Handle of the frame buffer
    uint32_t fb; //!< id of the frame buffer
    uint32_t conn; //!< id of the connector
    uint32_t crtc; //!< id of the "monitor"
    drmModeCrtc* saved_crtc; //!< drm internal datastructure for the crtc
};

/**
 * Variable which holds the type information about the ws_array type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_MONITOR;

/**
 * Allocate a new, initialized array object
 *
 * @memberof ws_monitor
 *
 * @return New monitor object, initialized. NULL on failure
 */
struct ws_monitor*
ws_monitor_new(void);

#endif // __WS_OBJECTS_MONITOR_H__

/**
 * @}
 */

/**
 * @}
 */
