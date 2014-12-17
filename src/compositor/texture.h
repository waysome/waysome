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

#ifndef __WS_TEXTURE__
#define __WS_TEXTURE__

#include <GLES2/gl2.h>
#include <wayland-util.h>

#include "objects/object.h"

/**
 * Texture for dynamic area allocation
 *
 * This datatype represents a texture, which may have a number of texture areas
 * associated with it, ensuring that the areas do not overlap.
 *
 * @extends ws_object
 *
 * @copydoc texture_areas
 */
struct ws_texture {
    struct ws_object obj;   //!< @protected Base class.
    GLuint texture;         //!< @protected underlying texture
};

/**
 * Variable which holds the type information about the ws_texture type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_TEXTURE;

/**
 * Initialize a texture
 *
 * This function initializes a texture, which includes generating the texture.
 *
 * @memberof ws_texture
 *
 * @return 0 if the operation was successful, a negative error code otherwise
 */
int
ws_texture_init(
    struct ws_texture* self //!< object to initialize
);

/**
 * Create a texture
 *
 * This function creates a texture
 *
 * @memberof ws_texture
 *
 * @return a newly created texture, or `NULL`
 */
struct ws_texture*
ws_texture_new();

/**
 * Bind the texture
 *
 * This function binds the texture, e.g. to make it availible for blitting.
 * Essentially calls `glBindTexture()`
 *
 * @memberof ws_texture
 */
int
ws_texture_bind(
    struct ws_texture* self,    //!< object to modify
    GLenum target               //!< target to bind the texture to
);

#endif // __WS_TEXTURE__

/**
 * @}
 */

