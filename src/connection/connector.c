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
}

void
ws_connector_deinit(
    struct ws_connector* self
){
}

int
ws_connector_read(void){ 
    return 0;  
}

void
ws_connector_flush(
    struct ws_connector* self
){
}
