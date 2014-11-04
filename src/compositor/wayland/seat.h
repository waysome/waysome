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

#ifndef __WS_WAYLAND_SEAT_H__
#define __WS_WAYLAND_SEAT_H__

#include "objects/wayland_obj.h"
#include "util/wayland.h"

/**
 * Waysome's implementation of ws_wayland_seat
 *
 * This struct represents a ws_wayland_seat
 *
 * @extends ws_wayland_obj
 */
struct ws_wayland_seat {
    struct ws_wayland_obj wl_obj; //!< @protected Base class.
    //!< @todo: Set to real structs once libinput is added
    //struct devices;
    //struct capabilities;
};


/**
 * Initialize the seat interface and register it to wayland
 *
 * @memberof ws_wayland_seat
 *
 * @return 0 on success, else an error code
 */
int
ws_wayland_seat_init(void);


#endif // __WS_WAYLAND_SEAT_H__

