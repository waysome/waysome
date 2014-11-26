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
#include "util/attributes.h"

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

/**
 * Initialize a new EGL buffer for use with a device
 *
 * @memberof ws_egl_buffer
 *
 * @return 0 if the operation was successful, a negative error code otherwise
 */
int
ws_egl_buffer_init(
    struct ws_egl_buffer* buffer,       //!< buffer to initialize
    struct ws_framebuffer_device* dev,  //!< frame buffer device to bind to
    EGLConfig egl_conf,                 //!< config to apply
    uint32_t width,                     //!< width of the buffer
    uint32_t height,                    //!< height of the buffer
    uint32_t format                     //!< format of the buffer (from GBM)
)
__ws_nonnull__(1)
;

/**
 * Create a new EGL buffer from a gbm device, an EGL display and config
 *
 * @memberof ws_egl_buffer
 *
 * @return a newly created EGL buffer
 */
struct ws_egl_buffer*
ws_egl_buffer_new(
    struct ws_framebuffer_device* dev,  //!< frame buffer device to bind to
    EGLConfig egl_conf,                 //!< config to apply
    uint32_t width,                     //!< width of the buffer
    uint32_t height,                    //!< height of the buffer
    uint32_t format                     //!< format of the buffer (from GBM)
)
__ws_nonnull__(1)
;

#endif // __WS_EGL_BUFFERE_H__

