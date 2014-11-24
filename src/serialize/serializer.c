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

#include "objects/message/message.h"
#include "serialize/serializer.h"

#include <errno.h>

ssize_t
ws_serialize(
    struct ws_serializer* self,
    char* buf,
    size_t nbuf,
    struct ws_message* msg
) {
    ssize_t offset = 0;

    // try to clear the old message, if present
    if (self->buffer) {
        offset = self->serialize(self, buf, nbuf);
        if (offset < 0) {
            return offset;
        }

        // check whether we successfully flushed the message
        if (self->buffer) {
            return -EAGAIN;
        }

        // update buf and nbuf
        buf += offset;
        nbuf -= offset;
    }

    // check whether the message is
    if (!msg) {
        return offset;
    }

    // now try to serialize the message we have now
    ws_object_lock_write(&msg->obj);
    self->buffer = getref(msg);
    ssize_t retval = self->serialize(self, buf, nbuf);
    ws_object_unlock(&msg->obj);
    if (retval < 0) {
        return retval;
    }

    return retval + offset;
}

void
ws_serializer_deinit(
    struct ws_serializer* self
) {
    ws_object_unref(&self->buffer->obj);

    if (self->deinit) {
        self->deinit(self->state);
    }
}

