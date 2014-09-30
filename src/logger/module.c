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
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/module.h"
#include "util/cleaner.h"

/**
 * Logger type
 */
struct logger {
    pthread_mutex_t loglock; //!< Log mutex for synchronizing logging
};


static struct logger* logger = NULL;

/**
 * Cleanup function for the logger singleton
 *
 * Does the cleanup on exit of the software.
 */
static void
cleanup_logger(
    void* logger
) {
    struct logger* l = (struct logger*) logger;

    if (l) {
        pthread_mutex_destroy(&l->loglock);
        free(l);
    }
}

/*
 *
 * Interface implementation
 *
 */

int
ws_logger_new(void)
{
    if (!logger) {
        logger = calloc(1, sizeof(*logger));

        if (logger) {
            pthread_mutex_init(&logger->loglock, NULL);
        } else {
            return -ENOMEM;
        }
        ws_cleaner_add(cleanup_logger, logger);
    }

    return 0;
}

void
ws_log(
    struct ws_logger_context* const ctx,
    char* fmt,
    ...
) {
    if (!logger) {
        return;
    }
    char* __fmt = fmt;
    va_list list;
    va_start(list, fmt);

    bool used_prefix = false;

    if (ctx) {
        __fmt = calloc(1, strlen(ctx->prefix) + strlen(fmt) + 1);

        if (__fmt) {
            sprintf(__fmt, "%s%s", ctx->prefix, fmt);
            used_prefix = true;
        }
    }

    pthread_mutex_lock(&logger->loglock);
    fprintf(stderr, __fmt, list);
    pthread_mutex_unlock(&logger->loglock);

    va_end(list);

    if (used_prefix) {
        free(__fmt);
    }
}

/*
 * void
 * ws_log_str(
 *     struct ws_string* const str
 * );
 *
 */
