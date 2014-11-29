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
#include <stdlib.h>
#include <unistd.h>

#include "logger/module.h"
#include "util/exec.h"

static struct ws_logger_context logger_ctx = { .prefix = "[exec] " };

int
ws_exec(
    const char* call,
    char* const argv[]
) {
    pid_t pid = fork();

    if (pid == -1) {
        return -errno;
    }

    if (pid == 0) {
        // we're the child. We should drop our privileges first
        if (seteuid(getuid()) < 0) {
            exit(errno);
        }
        if (setegid(getgid()) < 0) {
            exit(errno);
        }

        // now execvp()
        if (execvp(call, argv) == -1) {
            exit(errno);
        }
    }

    // parent, log that it worked
    ws_log(&logger_ctx, LOG_INFO, "Successfully started %s", call);

    return 0;
}

