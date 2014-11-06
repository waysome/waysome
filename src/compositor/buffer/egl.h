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

#ifndef __WS_EGL_BUFFERE_H__
#define __WS_EGL_BUFFERE_H__

#include <EGL/egl.h>

#include "compositor/buffer/buffer.h"

// forward declarations
struct gbm_surface;
struct ws_framebuffer_device;

/**
 * EGL surface type
 *
 * @extends ws_buffer
 */
struct ws_egl_buffer {
    struct ws_buffer buf;               //!< @protected base class
    struct ws_framebuffer_device* dev;  //!< @protected ref to the fb dev
    struct gbm_surface* gbm_surf;       //!< @protected internal gbm surface
    EGLSurface egl_surf;                //!< @protected internal EGL surface
    uint32_t width;                     //!< @protected cached width
    uint32_t height;                    //!< @protected cached height
    uint32_t format;                    //!< @protected cached format
};

/**
 * Variable which holds the type information about the ws_egl_buffer type
 */
extern ws_buffer_type_id WS_OBJECT_TYPE_ID_EGL_BUFFER;

#endif // __WS_EGL_BUFFERE_H__

