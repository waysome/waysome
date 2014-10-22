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

#include <dirent.h>
#include <errno.h>
#include <ev.h>
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "input/module.h"
#include "logger/module.h"
#include "objects/set.h"

static struct ws_logger_context log_ctx = { .prefix = "[Input] " };

static struct ws_set devices;

#define INPUT_PATH "/dev/input/"
#define DEVICE_NAME "event"
#define MAX_FULL_PATH_LEN 100

/**
 * The callback used by libev to watch the INPUT_PATH directory
 */
static void
watch_devices(
    struct ev_loop* loop, //!< loop on which the callback was called
    ev_io* watcher, //!< watcher which triggered the update
    int revents //!< events
) {
    char buffer[1024]
        __attribute__((aligned(__alignof__(struct inotify_event))));

    struct inotify_event* event = (struct inotify_event*) buffer;
    ssize_t len = read(watcher->fd, buffer, sizeof(buffer));

    if (len == -1 && errno != EAGAIN) {
        ws_log(&log_ctx, LOG_CRIT,
                "Could not read from libev filedescriptor");
        return;
    }

    if (len <= 0) {
        return;
    }

    while (event < (struct inotify_event*) &buffer[len]) {
        if (event->mask & IN_CREATE) {
            ws_log(&log_ctx, LOG_DEBUG, "New file was created %s", event->name);
        }
        event = (struct inotify_event*)
            (((char*) event) + event->len + sizeof(*event));
    }


}

/**
 * Checks if the file is a file suitable for inclusion as a device file
 *
 * @returns true if it is suitable, false if not
 */
static bool
filter_name(
    const char* name
) {
    if (!(strcmp(".", name) && strcmp("..", name))) {
        return false;
    }
    // strcmp also makes note of the length between the strings, we only want
    // it to check the beginning of the file name, in this case it has to match
    // "event*"
    if (strncmp(DEVICE_NAME, name, sizeof(DEVICE_NAME)-1)) {
        return false;
    }
    return true;
}

/**
 * Iterates over the existing devices at startup to make a list which can be
 * complete later
 */
static int
find_initial_devices(void) {
    DIR* d;
    struct dirent* dir;

    d = opendir(INPUT_PATH);

    if (!d) {
        ws_log(&log_ctx, LOG_CRIT, "Could not open input device directory");
        return -1;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Starting listing of existing devices");


    while ((dir = readdir(d)) != NULL) {
        if (!filter_name(dir->d_name)) {
            ws_log(&log_ctx, LOG_DEBUG, "Skipping directory %s", dir->d_name);
            continue;
        }
        char full_path[MAX_FULL_PATH_LEN] = INPUT_PATH;
        strncat(full_path, dir->d_name, MAX_FULL_PATH_LEN - 1 - sizeof(INPUT_PATH));

        int fd = open(full_path, O_RDONLY | O_NONBLOCK);

        if (fd < 0) {
            ws_log(&log_ctx, LOG_ERR, "Could not open event file %s %s",
                    full_path, strerror(errno));
            continue;
        }

        struct libevdev* dev;
        libevdev_new_from_fd(fd, &dev);
        if (!dev) {
            ws_log(&log_ctx, LOG_ERR, "Could not create evdev struct %s",
                    dir->d_name);
            close(fd);
            continue;
        }

        ws_log(&log_ctx, LOG_DEBUG, "File %s is a mouse: %d keyboard: %d",
                full_path, libevdev_has_event_type(dev, EV_REL),
                libevdev_has_event_type(dev, EV_KEY));
        libevdev_free(dev);
        close(fd);

    }
    closedir(d);
    return 0;
}


int
ws_input_init(void)
{
    ws_set_init(&devices);

    int inotfd = inotify_init1(IN_NONBLOCK);

    if (inotfd < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not start inotify");
        return inotfd;
    }

    int watchfd = inotify_add_watch(inotfd, INPUT_PATH,
            IN_CREATE | IN_DELETE);

    if (watchfd < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not watch input devices");
        return watchfd;
    }

    struct ev_io* watcher = calloc(1, sizeof(*watcher));
    ev_io_init(watcher, watch_devices, inotfd, EV_READ);

    struct ev_loop* default_loop = ev_default_loop(EVFLAG_AUTO);
    if (!default_loop) {
        return -1;
    }

    ev_io_start(default_loop, watcher);
    ws_log(&log_ctx, LOG_DEBUG, "Started ev_loop to watch devices");

    int retval = find_initial_devices();

    if (retval < 0) {
        return retval;
    }

    return 0;
}

