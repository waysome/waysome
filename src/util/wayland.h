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

#ifndef __WS_UTIL_WAYLAND_H__
#define __WS_UTIL_WAYLAND_H__

#include <wayland-server.h>

/**
 * Initialize, lock and return the display
 *
 * This function will return the recursively locked wl_display, to prevent other
 * threads from interfering with the caller's actions on that object.
 * Callers will need to call ws_wayland_release_display() once they are done.
 *
 * This function imposes singleton behavior.
 * The display is initialized only once.
 * It is save to call this function multiple times.
 *
 * @warning locks the display for other threads recursively.
 *          Call ws_wayland_release_display() to release the lock.
 *
 * @return initialized display or NULL
 */
struct wl_display*
ws_wayland_acquire_display(void);

/**
 * Release the lock on the display singleton
 *
 * This function releases the display singleton, previously locked by a call
 * to ws_wayland_acquire_display()
 *
 * @warning call this function only if you called ws_wayland_acquire_display().
 */
void
ws_wayland_release_display(void);


#endif // __WS_UTIL_CLEANER_H__
