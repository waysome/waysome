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
 * @addtogroup compositor "Compositor"
 *
 * @{
 */

#ifndef __WS_OBJECTS_MONITOR_H__
#define __WS_OBJECTS_MONITOR_H__

#include <pthread.h>
#include <stdbool.h>
#include <xf86drmMode.h>

#include "compositor/buffer/frame.h"
#include "compositor/monitor_mode.h"
#include "objects/object.h"
#include "objects/set.h"

/**
 * ws_monitor type definition
 *
 * @extends ws_object
 *
 * The ws_monitor
 */
struct ws_monitor {
    struct ws_object obj; //!< @protected Base class.
    bool connected; //!< @public is the monitor connected?
    int id; //!< @public the id of the monitor relative to the fb_dev

    struct ws_frame_buffer* buffer; //!< @public The frame buffer

    struct ws_framebuffer_device* fb_dev; //!< @public Framebuffer Device
    struct ws_monitor_mode* current_mode;
    uint32_t handle; //!< @public Handle of the frame buffer
    uint32_t fb; //!< @public id of the frame buffer
    uint32_t conn; //!< @public id of the connector
    uint32_t crtc; //!< @public id of the "monitor"
    drmModeCrtc* saved_crtc; //!< @public drm internal datastructure for crtc

    struct ws_set surfaces; //!< @public
    struct ws_set modes;
    int mode_count;

    struct wl_global* global;
    struct wl_resource* resource;
    int phys_width;
    int phys_height;
};

/**
 * Variable which holds the type information about the ws_monitor type
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
 * @memberof ws_monitor
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

/**
 * Set the mode of the monitor to the given id
 *
 * @memberof ws_monitor
 */
void
ws_monitor_set_mode_with_id(
    struct ws_monitor* self,
    int id
);

/**
 * Adds a monitor_mode to this monitor
 *
 * @memberof ws_monitor
 *
 * @warning DO NOT USE! This is not yet safe as critical features are to be
 * implemented and it will stop waysome to save your Graphics Card
 *
 * @returns the newly created mode
 */
struct ws_monitor_mode*
ws_monitor_add_mode(
    struct ws_monitor* self,
    int width,
    int height
);

/**
 * Adds a monitor_mode to this monitor by copying an existing one
 *
 * @memberof ws_monitor
 *
 * @returns the newly copied mode
 */
struct ws_monitor_mode*
ws_monitor_copy_mode(
    struct ws_monitor* self,
    struct _drmModeModeInfo const* src
);

/**
 * Publishes the monitor through the wayland protocol
 *
 * @memberof ws_monitor
 */
void
ws_monitor_publish(
    struct ws_monitor* self
);

#endif // __WS_OBJECTS_MONITOR_H__

/**
 * @}
 */

