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
 * @addtogroup utils "(internal) utilities"
 *
 * @{
 */

/**
 * @addtogroup utils_wayland "wayland singleton and utilities"
 *
 * Utilities to make the ugnlier parts better handable and convertable.
 *
 * @{
 */

#ifndef __WS_UTIL_EGL_H__
#define __WS_UTIL_EGL_H__

#include <EGL/egl.h>
#include <wayland-server-protocol.h>

/**
 * Mapping between wayland related formats and egl formats
 */
struct ws_egl_fmt {
    enum wl_shm_format shm_fmt; //!< @public shared memory format
    struct {
        EGLenum fmt;            //!< @public egl format
        EGLenum type;           //!< @public egl type
    } egl;                      //!< @public egl format and type
    uint8_t bbp;                //!< @public bytes per pixel
};

/**
 * Query a mapping by the shared memory format
 *
 * @return a mapping with the specified shm format or `NULL`, if no such
 *         mapping exists
 */
struct ws_egl_fmt const*
ws_egl_fmt_from_shm_fmt(
    enum wl_shm_format shm_fmt  //!< shared format for which to get the mapping
);

#endif

/**
 * @}
 */

/**
 * @}
 */

