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

#include <png.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xf86drmMode.h>

#include "compositor/buffer/buffer.h"
#include "compositor/buffer/image.h"
#include "logger/module.h"
#include "util/arithmetical.h"

/*
 *
 * Forward declarations
 *
 */

/**
 * Deinit callback for buffer type
 */
static bool
deinit_buffer(
    struct ws_object* self
);

/**
 * Compare callback for buffer type
 */
static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

/**
 * Data getter callback for buffer type
 */
static void*
get_data(
    struct ws_buffer const* self
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

ws_buffer_type_id WS_OBJECT_TYPE_ID_IMAGE_BUFFER = {
    .type = {
        .supertype  = (ws_object_type_id*)&WS_OBJECT_TYPE_ID_BUFFER,
        .typestr    = "ws_image_buffer",

        .hash_callback = NULL,

        .deinit_callback = deinit_buffer,
        .dump_callback = NULL,
        .run_callback = NULL,
        .cmp_callback = cmp_buffer,

        .attribute_table = NULL,
        .function_table = NULL,
    },
    .get_data = get_data,
    .get_width = get_width,
    .get_height = get_height,
    .get_stride = get_stride,
    .get_format = get_format,
    .get_bpp = get_bpp,
    .begin_access = NULL,
    .end_access = NULL,
};

/*
 *
 * interface implementation
 *
 */

struct ws_image_buffer*
ws_image_buffer_from_png(
    const char* filename
) {
    struct ws_image_buffer* buff = ws_image_buffer_new();
    png_image img;
    memset(&img, 0, sizeof(img));
    img.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_file(&img, filename)) {
        ws_log(NULL, LOG_ERR, "Could not read file: '%s' error: %s.",
                filename, img.message);
        return NULL;
    }

    img.format = PNG_FORMAT_BGRA;
    buff->buffer = calloc(1, PNG_IMAGE_SIZE(img));
    buff->height = img.height;
    buff->width = img.width;
    buff->stride = PNG_IMAGE_ROW_STRIDE(img);

    if (!buff->buffer) {
        ws_log(NULL, LOG_ERR, "Could not allocate memory for image: '%s'",
                filename);
        png_image_free(&img);
        return NULL;
    }

    if (!png_image_finish_read(&img, NULL, buff->buffer, 0, NULL)) {
        ws_log(NULL, LOG_ERR, "Could not read file: '%s'.", filename);
        return NULL;
    }
    return buff;
}

struct ws_image_buffer*
ws_image_buffer_new(void)
{
    struct ws_image_buffer* tmp = calloc(1, sizeof(*tmp));
    ws_buffer_init(&tmp->obj);
    tmp->obj.obj.settings |= WS_OBJECT_HEAPALLOCED;
    tmp->obj.obj.id = (ws_object_type_id *)&WS_OBJECT_TYPE_ID_IMAGE_BUFFER;
    return tmp;
}

/*
 *
 * Implementations
 *
 */

static bool
deinit_buffer(
    struct ws_object* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    free(buff->buffer);
    return true;
}

static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_image_buffer* buff1 = (struct ws_image_buffer*) obj1;
    struct ws_image_buffer* buff2 = (struct ws_image_buffer*) obj2;
    int cmp = strcmp(buff1->path, buff2->path);
    return signum(cmp);
}

static void*
get_data(
    struct ws_buffer const* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    return buff->buffer;
}

static int32_t
get_height(
    struct ws_buffer const* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    return  buff->height;
}

static int32_t
get_width(
    struct ws_buffer const* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    return buff->width;
}

static int32_t
get_stride(
    struct ws_buffer const* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    return buff->stride;
}

static uint32_t
get_format(
    struct ws_buffer const* self
) {
    //struct ws_image_buffer* buff = (struct ws_image_buffer*) self;
    return 0; //!< @todo: unused (But maybe of use later)
}

static uint32_t
get_bpp(
    struct ws_buffer const* self
) {
    // This is the amount of bytes per pixel (BGRA)
    return 4;
}

