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
#include <stdio.h>
#include <unistd.h>

#include "connection/connector.h"

#include "connection/connbuf.h"

#define BUFFSIZE 4096

int
ws_connector_init(
    struct ws_connector* self,
    int fd
){
    int res;
    res = ws_connbuf_init(&self->inbuf, BUFFSIZE);
    if (res != 0) {
        return res;
    }

    res = ws_connbuf_init(&self->outbuf, BUFFSIZE);
    if (res != 0) {
        return res;
    }

    self->readonly = false;
    self->fd = fd;

    return 0;
}

int ws_connector_init_readonly(
    struct ws_connector* self,
    int fd
){
    int res;
    res = ws_connbuf_init(&self->inbuf, BUFFSIZE);
    if (res != 0) {
        return res;
    }

    self->readonly = true;
    self->fd = fd;

    return 0;
}

void
ws_connector_deinit(
    struct ws_connector* self
){
    ws_connbuf_deinit(&self->inbuf);

    if (!self->readonly) {
        ws_connbuf_deinit(&self->outbuf);
    }

    if (self->fd >= 0) {
        close(self->fd);
    }
}

int
ws_connector_read(
    struct ws_connector* self
){
    char* start;
    int res;
    size_t remaining_mem; //amount of free memory in outbuf

    remaining_mem = ws_connbuf_available(&self->inbuf);
    start = ws_connbuf_reserve(&self->inbuf, remaining_mem);

    res = read(self->fd, start, remaining_mem);
    if (res == 0) {
        // we hit the end of file
        ws_connbuf_unblock(&self->inbuf);
        return EOF;
    }

    if (res < 0) {
        ws_connbuf_unblock(&self->inbuf);
        return -errno;
    }

    res = ws_connbuf_append(&self->inbuf, res);
    if (res != 0) {
        return res;
    }

    return 0;
}

int
ws_connector_flush(
    struct ws_connector* self
){
    if (self->readonly) {
        return -1;
    }

    size_t used_mem = self->outbuf.data;
    int res;

    res = write(self->fd, self->outbuf.buffer, used_mem);
    if (res < 0 ) {
        return -errno;
    }

    res = ws_connbuf_discard(&self->outbuf, used_mem);
    if (res != 0) {
        return res;
    }

    return 0;
}
