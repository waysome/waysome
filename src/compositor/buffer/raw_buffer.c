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

#include <string.h>

#include "compositor/buffer/buffer.h"
#include "compositor/buffer/raw_buffer.h"
#include "logger/module.h"
#include "util/arithmetical.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Compare callback for buffer type
 */
static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/**
 * Height getter callback for buffer type
 */
static int32_t
get_height(
    struct ws_buffer const* self
);

/**
 * Width getter callback for buffer type
 */
static int32_t
get_width(
    struct ws_buffer const* self
);

/**
 * Stride getter callback for buffer type
 */
static int32_t
get_stride(
    struct ws_buffer const* self
);

/**
 * Format getter callback for buffer type
 */
static uint32_t
get_format(
    struct ws_buffer const* self
);

/**
 * BPP getter callback for buffer type
 */
static uint32_t
get_bpp(
    struct ws_buffer const* self
);

/*
 *
 * Type information variable
 *
 */

ws_buffer_type_id WS_OBJECT_TYPE_ID_RAW_BUFFER = {
    .type = {
        .supertype          = (ws_object_type_id*)&WS_OBJECT_TYPE_ID_BUFFER,
        .typestr            = "ws_raw_buffer",

        .hash_callback      = NULL,

        .deinit_callback    = NULL,
        .cmp_callback       = cmp_buffer,

        .attribute_table    = NULL,
        .function_table     = NULL,
    },
    .get_data       = NULL,
    .get_width      = get_width,
    .get_height     = get_height,
    .get_stride     = get_stride,
    .get_format     = get_format,
    .get_bpp        = get_bpp,
    .begin_access   = NULL,
    .end_access     = NULL,
};

/*
 *
 * Implementations
 *
 */

static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_raw_buffer* buff1 = (struct ws_raw_buffer*) obj1;
    struct ws_raw_buffer* buff2 = (struct ws_raw_buffer*) obj2;
    int cmp = strcmp(buff1->path, buff2->path);
    return signum(cmp);
}

static int32_t
get_height(
    struct ws_buffer const* self
) {
    struct ws_raw_buffer* buff = (struct ws_raw_buffer*) self;
    return  buff->height;
}

static int32_t
get_width(
    struct ws_buffer const* self
) {
    struct ws_raw_buffer* buff = (struct ws_raw_buffer*) self;
    return buff->width;
}

static int32_t
get_stride(
    struct ws_buffer const* self
) {
    struct ws_raw_buffer* buff = (struct ws_raw_buffer*) self;
    return buff->stride;
}

static uint32_t
get_format(
    struct ws_buffer const* self
) {
    //struct ws_raw_buffer* buff = (struct ws_raw_buffer*) self;
    return 0; //!< @todo: unused (But maybe of use later)
}

static uint32_t
get_bpp(
    struct ws_buffer const* self
) {
    // This is the amount of bytes per pixel (BGRA)
    return 4;
}

