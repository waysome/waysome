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

#include "input/hotkeys.h"
#include "input/input_device.h"
#include "input/module.h"
#include "input/utils.h"
#include "logger/module.h"
#include "objects/set.h"
#include "util/cleaner.h"

static struct ws_logger_context log_ctx = { .prefix = "[Input/Device] " };

#define INPUT_PATH "/dev/input/"
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
        if (!ws_input_filter_event_device_name(dir->d_name)) {
            ws_log(&log_ctx, LOG_DEBUG, "Skipping directory %s", dir->d_name);
            continue;
        }
        char full_path[MAX_FULL_PATH_LEN] = INPUT_PATH;
        strncat(full_path, dir->d_name,
                MAX_FULL_PATH_LEN - 1 - sizeof(INPUT_PATH));

        int fd = open(full_path, O_RDONLY | O_NONBLOCK);

        if (fd < 0) {
            ws_log(&log_ctx, LOG_ERR, "Could not open event file %s %s",
                    full_path, strerror(errno));
            continue;
        }

        struct ws_input_device* new_dev = ws_input_device_new(fd);

        ws_set_insert(&ws_input_ctx.devices, &new_dev->obj);

    }
    closedir(d);
    return 0;
}

static void
cleanup_input(
    void* dummy
) {
    int size = ws_set_cardinality(&ws_input_ctx.devices);

    while (size--) {
        struct ws_input_device* dev =
            (struct ws_input_device*) ws_set_select_any(&ws_input_ctx.devices);

        ws_set_remove(&ws_input_ctx.devices, &dev->obj);

        libevdev_free(dev->dev);
        ev_io_stop(ev_default_loop(EVFLAG_AUTO), &dev->watcher);
        close(dev->fd);
        ws_object_unref(&dev->obj);
    }

    ws_object_deinit(&ws_input_ctx.devices.obj);
}

int
ws_input_init(void)
{
    static bool is_init = false;

    if (is_init) {
        return 0;
    }
    ws_set_init(&ws_input_ctx.devices);

    int hk_init = ws_hotkeys_init();
    if (hk_init < 0) {
        ws_log(&log_ctx, LOG_CRIT, "Could not start hotkey subsystem");
        return hk_init;
    }

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

    ws_cleaner_add(cleanup_input, NULL);

    is_init = true;
    return 0;
}

