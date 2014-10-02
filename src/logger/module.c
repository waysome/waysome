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
#include "util/attributes.h"
#include "util/cleaner.h"

/**
 * Logger type
 */
static struct {
    pthread_mutex_t loglock; //!< Log mutex for synchronizing logging
} logger;

/**
 * Cleanup function for the logger singleton
 *
 * Does the cleanup on exit of the software.
 */
static void
cleanup_logger(
    void* etc __ws_unused__
) {
    pthread_mutex_destroy(&logger.loglock);
}

/*
 *
 * Interface implementation
 *
 */

int
ws_logger_init(void)
{
    static bool is_used = false;

    if (is_used) {
        return 0;
    }

    pthread_mutex_init(&logger.loglock, NULL);
    ws_cleaner_add(cleanup_logger, NULL);
    is_used = true;

    return 0;
}

void
ws_log(
    struct ws_logger_context* const ctx,
    char* fmt,
    ...
) {
    va_list list;
    size_t pref_len = 0;
    size_t buf_len = strlen(fmt);

    if (ctx && ctx->prefix) {
        pref_len = strlen(ctx->prefix);
        buf_len += pref_len;
    }

    buf_len += 2; // newline-char and zero-byte

    char buf[buf_len];

    memcpy(buf + pref_len, fmt, strlen(fmt));

    if (ctx && ctx->prefix) {
        memcpy(buf, ctx->prefix, pref_len);
    }

    buf[buf_len - 2] = '\n';
    buf[buf_len - 1] = '\0';

    va_start(list, fmt);

    pthread_mutex_lock(&logger.loglock);
    vfprintf(stderr, (char *) buf, list);
    pthread_mutex_unlock(&logger.loglock);

    va_end(list);
}

/*
 * void
 * ws_log_str(
 *     struct ws_string* const str
 * );
 *
 */
