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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util/arithmetical.h"
#include "util/config_file.h"

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


/*
 *
 * Interface implementation
 *
 */

int
ws_config_load(void) {
    //!< @todo implement
    return -1;
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
        int fd = open(path, O_RDONLY);
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

