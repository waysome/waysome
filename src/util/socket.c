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
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "logger/module.h"
#include "util/socket.h"

#define XDG_RUNTIME_DIR "XDG_RUNTIME_DIR"
#define UNIX_PATH_MAX 108

static struct ws_logger_context log_ctx = { .prefix = "[Sockets Utils] " };

/**
 * Build a path with the file name, using the xdg runtime dir
 *
 * @return zero on success or negative errno.h on failure
 */
static int
build_path(
    const char* name, //!< The name of the file
    char* dest //!< The destination for the path, must =< UNIX_PATH_MAX bytes
);

static void
socket_build_connection_cb(
    struct ev_loop* loop,
    struct ev_io* io,
    int revents
) {
    struct ws_socket* s = (struct ws_socket*) io->data;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);

    if (revents & EV_ERROR) {
        ws_log(&log_ctx, LOG_WARNING, "Libev error in accept callback!");
        return;
    }

    memset(&addr, 0, len);
    int fd = accept(s->fd, (struct sockaddr*) &addr, &len);
    if (fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            ws_log(&log_ctx, LOG_ERR, "Could not accept client: %d", errno);
        }
        return;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Connection established!");

    int res = s->createconn_cb(fd);
    if (res == 0) {
        ws_log(&log_ctx, LOG_DEBUG, "Connection creation successfull.");
    } else {
        ws_log(&log_ctx, LOG_DEBUG, "Connection creation failed.");
        close(fd);
    }
}

int
ws_socket_init(
    struct ws_socket* s,
    int (*createconn_cb)(int fd),
    char const* name,
    int backlog
) {
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (!loop) {
        ws_log(&log_ctx, LOG_WARNING, "Could not initialize ev loop!");
        free(s);
        return -1;
    }

    s->fd = ws_socket_create(name);
    if (s->fd < 0) {
        return s->fd;
    }

    int res = listen(s->fd, backlog);
    if (res < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not listen.");
        return -errno;
    }

    s->createconn_cb    = createconn_cb;
    s->io.data          = s; // self-ref here, to be forward compatible
    s->path             = name;

    ev_io_init(&s->io, socket_build_connection_cb, s->fd, EV_READ);
    ev_io_start(loop, &s->io);

    return 0;
}

int
ws_socket_deinit(
    struct ws_socket* sock
) {
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    ev_io_stop(loop, &sock->io);

    char sockname[UNIX_PATH_MAX];
    memset(sockname, 0, UNIX_PATH_MAX);

    int retval = build_path(sock->path, sockname);
    if (retval < 0) {
        return retval;
    }

    if (unlink(sockname) != 0) {
        ws_log(&log_ctx, LOG_WARNING, "Could not unlink() -> errno: %i", errno);
    }
    return (close(sock->fd) != 0) ? -errno : 0;
}

int
ws_socket_create(
    char const* name
) {
    /* Sock(et)s!
     *  mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm
     *  | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |
     *  | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |
     *  \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \
     *   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)
     */

    int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sock < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not create socket");
        return sock;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    int retval = build_path(name, addr.sun_path);
    if (retval < 0) {
        ws_log(&log_ctx, LOG_ERR, "Couldn't create socket %s", addr.sun_path);
        return retval;
    }

    int res = bind(sock, (struct sockaddr*) &addr, sizeof(addr));

    if (res == -1 && errno == EACCES) {
        unlink(addr.sun_path);
        res = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
    }

    if (res < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not bind.");
        return -errno;
    }

    return sock;
}

static int
build_path(
    const char* name,
    char* dest
) {
    char* xdg_env = getenv(XDG_RUNTIME_DIR);

    if (!xdg_env) {
        ws_log(&log_ctx, LOG_WARNING, "XDG_RUNTIME_DIR is not set!");
        xdg_env = "/tmp";
    }

    int length = snprintf(dest, UNIX_PATH_MAX, "%s/%s", xdg_env, name);

    if (length < 0) {
        return length;
    }

    return 0;
}

