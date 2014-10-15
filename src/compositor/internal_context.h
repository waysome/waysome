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

#ifndef __WS_COMPOSITOR_INTERNAL_CONTEXT_H__
#define __WS_COMPOSITOR_INTERNAL_CONTEXT_H__

#include "objects/set.h"
#include "compositor/framebuffer_device.h"
#include "compositor/cursor.h"

/**
 * Internal compositor context
 *
 * This context holds the internal state of the compositor.
 */
extern struct ws_compositor_context {
    struct ws_framebuffer_device* fb; //<! The Framebuffer device
    struct ws_set monitors; //<! A set of monitors (connected or not)
    struct ws_monitor* conns; //<! A linked list of ws_monitors
    struct ws_cursor* cursor; //<! The cursor
} ws_comp_ctx;

// We make the object available for others to use
extern struct ws_logger_context log_ctx;


#endif // __WS_COMPOSITOR_INTERNAL_CONTEXT_H__
