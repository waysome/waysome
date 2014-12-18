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

#include <EGL/egl.h>

#include "compositor/texture.h"

/*
 *
 * Forward declaration
 *
 */

static bool
texture_deinit(
    struct ws_object* obj //!< object to deinit
);

/*
 *
 * Internal constants
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_TEXTURE = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_texture",

    .hash_callback = NULL,

    .deinit_callback = texture_deinit,
    .cmp_callback = NULL,
    .uuid_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};


/*
 *
 * Interface implementation
 *
 */

int
ws_texture_init(
    struct ws_texture* self
) {
    int res = ws_object_init(&self->obj);
    if (res < 0) {
        return res;
    }
    self->obj.id = &WS_OBJECT_TYPE_ID_TEXTURE;

    // get texture
    glGenTextures(1, &self->texture);

    return eglGetError() == GL_NO_ERROR ? 0 : -1;
}

struct ws_texture*
ws_texture_new() {
    struct ws_texture* retval = calloc(1, sizeof(*retval));
    if (ws_texture_init(retval) < 0) {
        return NULL;
    }
    retval->obj.settings |= WS_OBJECT_HEAPALLOCED;

    return retval;
}

int
ws_texture_bind(
    struct ws_texture* self,
    GLenum target
) {
    glBindTexture(target, self->texture);
    return 0;
}


/*
 *
 * Internal implementation
 *
 */

static bool
texture_deinit(
    struct ws_object* obj
) {
    struct ws_texture* self = (struct ws_texture*) obj;

    // get rid of texture
    glDeleteTextures(1, &self->texture);

    return true;
}

