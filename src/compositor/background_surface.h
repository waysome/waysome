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

#ifndef __WS_BACKGROUND_SERVICE_H__
#define __WS_BACKGROUND_SERVICE_H__

#include "compositor/buffer.h"

struct ws_image_buffer {
    struct ws_buffer obj;
    char* path;
    int bytesize;
    int height;
    int stride;
    int width;
    void* buffer;
};

/**
 * Variable which holds the type information about the ws_image_buffer type
 */

extern ws_buffer_type_id WS_OBJECT_TYPE_ID_IMAGE_BUFFER;

/**
 * Initialize a `ws_image_buffer` object
 *
 * @return an empty image buffer
 */
struct ws_image_buffer*
ws_image_buffer_new(void);

/**
 * Initialize a `ws_image_buffer` object and load the image
 * specified by the path argument
 *
 * @return a filled image buffer, or NULL on failure
 */
struct ws_image_buffer*
ws_image_buffer_from_png(
    const char* filename
);

#endif // __WS_BACKGROUND_SERVICE_H__
