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
 * @addtogroup objects_monitor_mode "Class: Monitor Mode type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_MONITOR_MODE_H__
#define __WS_OBJECTS_MONITOR_MODE_H__

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "objects/object.h"
#include "objects/set.h"

/**
 * ws_monitor type definition
 *
 * @extends ws_object
 *
 * The ws_monitor
 */
struct ws_monitor_mode {
    struct ws_object obj; //!< @protected Base class.
    int id; //!< @public the id of the mode
    drmModeModeInfo mode; //!< @public mode of the monitor
};

/**
 * Variable which holds the type information about the ws_array type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_MONITOR_MODE;

/**
 * Allocate a new, initialized array object
 *
 * @memberof ws_monitor
 *
 * @return New monitor object, initialized. NULL on failure
 */
struct ws_monitor_mode*
ws_monitor_mode_new(void);

#endif // __WS_OBJECTS_MONITOR_MODE_H__

/**
 * @}
 */

/**
 * @}
 */
