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
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "connection/config_file.h"
#include "connection/processor.h"
#include "serialize/json/deserializer.h"
#include "util/arithmetical.h"

#define HOME            "HOME"
#define XDG_CONFIG_DIRS "XDG_CONFIG_DIRS"
#define XDG_CONFIG_HOME "XDG_CONFIG_HOME"

/*
 *
 * Forward declarations
 *
 */

/**
 * Try all waysome configuration basenames
 *
 * @return a non-negative fd or a negative error number
 */
static int
tryopen_variants(
    char const* directory //!< Directory to search in
);

/**
 * `open()` the configuration
 *
 * @return a non-negative file descriptor or a negative error code
 */
static int
open_config(void);


/*
 *
 * Interface implementation
 *
 */

int
ws_connection_loadconf(void) {
    int fd = open_config();
    if (fd < 0) {
        return fd;
    }

    // create the deserializer
    struct ws_deserializer* deserializer;
    deserializer = ws_serializer_json_deserializer_new();
    if (!deserializer) {
        return -1;
    }

    struct ws_connection_manager* proc;
    proc = ws_connection_manager_new(fd, deserializer, NULL);
    if (!proc) {
        return -1;
    }
    ws_object_unref((struct ws_object*) proc);
    return 0;
}


/*
 *
 * Internal implementations
 *
 */

static int
tryopen_variants(
    char const* directory
) {
    static char const* variants[] = {
        "/waysome.conf",
        "/waysome.json",
        "/.waysome.conf",
        "/.waysome.json",
        "/waysome/waysome.conf",
        "/waysome/waysome.json",
    };

    struct stat statbuf;
    if ((stat(directory, &statbuf) != 0) || !S_ISDIR(statbuf.st_mode)) {
        return -1;
    }

    const size_t dirlen = strlen(directory);

    // iterate over all the variants
    char const** var = variants + ARYLEN(variants);
    while (var-- > variants) {
        // assemble the path
        char path[dirlen + strlen(*var) + 1];
        (void) strcpy(path, directory);
        (void) strcpy(path + dirlen, *var);

        // try to open the file
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }

        if ((fstat(fd, &statbuf) == 0) && S_ISREG(statbuf.st_mode)) {
            return fd;
        }

        close(fd);
    }

    return -ENOENT;
}

static int
open_config(void) {
    char* dir;
    int res;

    // check subdirectories of the home directory
    {
        char const* home = getenv(HOME);
        size_t homelen = strlen(home);
        char buf[homelen + 10]; // buffer to hold $HOME _and_ "/.config"

        // check the xdg config home first
        dir = getenv(XDG_CONFIG_HOME);
        if (!dir || (*dir == '\0')) {
            strcpy(buf, home);
            strcpy(buf + homelen, "/.config");
            dir = buf;
        }

        res = tryopen_variants(dir);
        if (res >= 0) {
            return res;
        }

        res = tryopen_variants(home);
        if (res >= 0) {
            return res;
        }
    }

    // check the xdg config directories
    {
        // setup all the buffers and temporaries
        char const* xdg_dirs = getenv(XDG_CONFIG_DIRS);
        if (!xdg_dirs) {
            xdg_dirs = "/etc/xdg";
        }

        char buf[strlen(xdg_dirs) + 1];
        (void) strcpy(buf, xdg_dirs);

        // do the actual work
        char* save;
        char* dir = strtok_r(buf, ":", &save);
        do {
            int res = tryopen_variants(dir);
            if (res >= 0) {
                return res;
            }
        } while (strtok_r(NULL, ":", &save));
    }

    // now look in /etc
    res = tryopen_variants("/etc");
    if (res >= 0) {
        return res;
    }

    // last resort ...
    return tryopen_variants(".");
}

