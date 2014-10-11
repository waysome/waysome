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

#include <pthread.h>
#include <stdbool.h>
#include <xf86drmMode.h>

#include "objects/object.h"
#include "objects/set.h"

/**
 * ws_monitor type definition
 *
 * The ws_monitor
 */
struct ws_monitor {
    struct ws_object obj; //!< Supertype
    bool connected; //!< is the monitor connected?

    struct ws_image_buffer* buffer; //!< The frame buffer

    struct ws_framebuffer_device* fb_dev; //!< Framebuffer Device
    drmModeModeInfo mode; //!< mode of the monitor
    uint32_t handle; //!< Handle of the frame buffer
    uint32_t fb; //!< id of the frame buffer
    uint32_t conn; //!< id of the connector
    uint32_t crtc; //!< id of the "monitor"
    drmModeCrtc* saved_crtc; //!< drm internal datastructure for the crtc

    struct ws_set surfaces;
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

/**
 * Get the set of suraces assigned to the monitor
 *
 * @warning you must hold a reference of the monitor while working on the set
 *          returned to prevent destruction of the set by another thread.
 *
 * @return the set of surfaces assigned to the monitor
 */
struct ws_set*
ws_monitor_surfaces(
    struct ws_monitor* self //!< the monitor to query
);

/**
 * Populate the monitor with a framebuffer, unless one already exists or
 * the monitor is not connected.
 *
 * @memberof ws_monitor
 */
void
ws_monitor_populate_fb(
    struct ws_monitor* self
);

#endif // __WS_OBJECTS_MONITOR_H__

/**
 * @}
 */

/**
 * @}
 */
