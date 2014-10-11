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

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "connection/connbuf.h"

int
ws_connbuf_init(
    struct ws_connbuf* self,
    size_t amount
) {
    if (amount == 0) {
        return -EINVAL;
    }

    self->buffer = calloc(amount, sizeof(char));
    if (!self->buffer) {
        return -ENOMEM;
    }

    self->size = amount;
    self->data = 0;
    self->blocked = false;

    return 0;
}

void
ws_connbuf_deinit(
    struct ws_connbuf* self
) {
    free(self->buffer);

    self->buffer = NULL;
    self->data = 0;
    self->size = 0;
    self->blocked = false;
}

char*
ws_connbuf_reserve(
    struct ws_connbuf* self,
    size_t amount
) {
    if (amount == 0) {
        return NULL;
    }

    self->blocked = true;

    if ((self->data + amount) > self->size) {
        return NULL;
    }

    return self->buffer + self->data;
}

int
ws_connbuf_append(
    struct ws_connbuf* self,
    size_t amount
) {
    if (amount == 0) {
        return -EINVAL;
    }

    if (!self->blocked) {
        return -EINTR;
    }

    if ((self->data + amount) > self->size) {
        return -ENOSPC;
    }

    self->data += amount;
    self->blocked = false;

    return 0;
}

