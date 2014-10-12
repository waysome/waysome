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

#include "objects/message.h"
#include "serialize/deserializer.h"

/**
 *
 * Interface implementation
 *
 */

ssize_t
ws_deserialize(
    struct ws_deserializer* self,
    struct ws_message** msg,
    char const* buf,
    size_t nbuf
) {
    // we don't have a message yet
    self->is_ready = false;

    // try to extract a message
    ssize_t retval = self->deserialize(self, buf, nbuf);

    // optimistic write
    *msg = NULL;
    if (self->is_ready) {
        // transfer object without getref
        *msg = self->buffer;
        self->buffer = NULL;
    }

    return retval;
}

void
ws_deserializer_deinit(
    struct ws_deserializer* self
) {
    ws_object_unref(&self->buffer->obj);

    if (self->deinit) {
        self->deinit(self->state);
    }
}

