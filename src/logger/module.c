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

#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>

#include "logger/module.h"
#include "util/cleaner.h"

static struct ws_logger* logger = NULL;

struct ws_logger*
ws_logger_new(void)
{
    if (!logger) {
        logger = calloc(1, sizeof(*logger));

        if (logger) {
            pthread_mutex_init(&logger->loglock, NULL);
        }
        ws_cleaner_add(cleanup_logger, logger);
    }

    return logger;
}

void
ws_log(
    struct ws_logger* const logger,
    struct ws_logger_context* const ctx,
    char* fmt,
    ...
) {
    /** @todo implement */
    return;
}

/*
 * void
 * ws_log_str(
 *     struct ws_string* const str
 * );
 *
 */
