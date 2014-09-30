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

#ifndef __WS_COMPOSITOR_MODULE_H__
#define __WS_COMPOSITOR_MODULE_H__

#include <stdint.h>
#include <stdlib.h>
#include <xf86drmMode.h>
/**
 * A framebuffer device
 */
struct ws_framebuffer_device {
    int fd; //<! The FileDescriptor
    const char* path; //<! The path to the given filedescriptor
};

/**
 * A connector, which under DRM translates to a Device+Output
 */
struct ws_monitor {
    struct ws_monitor* next;
    int connected;
    uint32_t width;
    uint32_t height;
    uint32_t stride;

    uint32_t size;
    uint8_t* map;

    drmModeModeInfo mode;
    uint32_t fb;
    uint32_t conn;
    uint32_t crtc;
    drmModeCrtc* saved_crtc;
};

/**
 * Initialise the compositor
 *
 * This function initialises the compositor.
 * This includes allocating resources and performing initial discovery.
 * The function takes care of initialising the compositor only once, it is save
 * to call this function multiple times.
 *
 *
 * @return 0 if the initialisation was successful, a negative error code on
 *         failure
 */
int
ws_compositor_init(void);

#endif // __WS_COMPOSITOR_MODULE_H__
