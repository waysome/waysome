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

#ifndef __WS_WL_REGION_H__
#define __WS_WL_REGION_H__

#include "compositor/wayland/buffer.h"
#include "objects/wayland_obj.h"


/**
 * Waysome's implementation of wl_region
 *
 * This struct represents a region
 */
struct ws_region {
    struct ws_wayland_obj wl_obj; //!< @protected Base class.
    struct ws_image_buffer img_buf; //!< @protected image buffer
};

/**
 * Variable which holds type information about the wl_region type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_REGION;

/**
 * Create a new region
 *
 * @memberof ws_region
 *
 * create a new region without any buffers attached to it
 */
struct ws_region*
ws_region_new(
    struct wl_client* client, //!< client requesting the region creation
    uint32_t serial //!< id of the newly created region
);

/**
 * Get a region from a resource
 *
 * Extracts the region from a resource.
 *
 * @memberof ws_region
 *
 * @return the region or NULL, if the resource is not a wl_region.
 */
struct ws_region*
ws_region_from_resource(
    struct wl_resource* resource
);

/**
 * Check if a given position is inside the input region
 *
 * @memberof ws_region
 */
bool
ws_region_inside(
    struct ws_region* region, //!< the region to check against
    int x, //!< the position
    int y //!< the position
);

#endif // __WS_WL_REGION_H__

