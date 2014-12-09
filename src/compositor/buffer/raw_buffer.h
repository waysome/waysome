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
 * @addtogroup compositor "Compositor"
 *
 * @{
 */

/**
 * @addtogroup compositor_buffer "Compositor Buffer"
 *
 * @{
 */

#ifndef __WS_RAW_BUFFER_H__
#define __WS_RAW_BUFFER_H__

#include "compositor/buffer/buffer.h"

/**
 * Abstract image buffer type, does not have any methods, implements just
 * callbacks for subtypes
 *
 * @extends ws_buffer
 */
struct ws_raw_buffer {
    struct ws_buffer obj;   //!< @protected Base class.
    char* path;             //!< @protected
    int bytesize;           //!< @protected
    int height;             //!< @protected
    int stride;             //!< @protected
    int width;              //!< @protected
    int size;               //!< @protected
};

/**
 * Variable which holds the type information about the ws_raw_buffer type
 */
extern ws_buffer_type_id WS_OBJECT_TYPE_ID_RAW_BUFFER;

#endif // __WS_RAW_BUFFER_H__

/**
 * @}
 */

/**
 * @}
 */

