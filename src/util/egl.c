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

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <wayland-server.h>

#include "util/arithmetical.h"
#include "util/egl.h"


/*
 *
 * Internal constants
 *
 */

static struct ws_egl_fmt const mappings[] = {
    {
        .shm_fmt = WL_SHM_FORMAT_RGBA8888,
        .egl = { .fmt = GL_RGBA8_OES, .type = GL_UNSIGNED_INT },
        .bpp = 4
    },
    {
        .shm_fmt = WL_SHM_FORMAT_RGBX8888,
        .egl = { .fmt = GL_RGBA8_OES, .type = GL_UNSIGNED_INT },
        .bpp = 4
    }
};


/*
 *
 * Interface implementation
 *
 */

struct ws_egl_fmt const*
ws_egl_fmt_from_shm_fmt(
    enum wl_shm_format shm_fmt
) {
    struct ws_egl_fmt const* mapping = mappings + ARYLEN(mappings);

    while (mapping-- > mappings) {
        if (mapping->shm_fmt == shm_fmt) {
            return mapping;
        }
    }

    return NULL;
}

struct ws_egl_fmt const*
ws_egl_fmt_get_rgba() {
    return mappings;
}

int
ws_egl_fmt_advertise(
    struct wl_display* display
) {
    int retval = -1;

    struct ws_egl_fmt const* mapping = mappings + ARYLEN(mappings);

    while (mapping-- > mappings) {
        if (wl_display_add_shm_format(display, mapping->shm_fmt)) {
            retval = 0;
        }
    }

    return retval;
}

void
ws_egl_flush_errors(void)
{
    while (glGetError() != GL_NO_ERROR);
}
