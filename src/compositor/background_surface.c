#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <xf86drmMode.h>

#include "logger/module.h"
#include "util/arithmetical.h"
#include "buffer.h"

#include "background_surface.h"

/*
 *
 * Forward declarations
 *
 */

static bool
deinit_buffer(
    struct ws_object* self
);

static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
);

static void*
get_data(
    struct ws_buffer* self
);

static int32_t
get_height(
    struct ws_buffer* self
);

static int32_t
get_width(
    struct ws_buffer* self
);

static int32_t
get_stride(
    struct ws_buffer* self
);

static uint32_t
get_format(
    struct ws_buffer* self
);

ws_buffer_type_id WS_OBJECT_TYPE_ID_IMAGE_BUFFER = {
    .type = {
        .supertype  = (ws_object_type_id*)&WS_OBJECT_TYPE_ID_BUFFER,
        .typestr    = "ws_image_buffer",

        .hash_callback = NULL,

        .init_callback = NULL,
        .deinit_callback = deinit_buffer,
        .dump_callback = NULL,
        .run_callback = NULL,
        .cmp_callback = cmp_buffer,
    },
    .get_data = get_data,
    .get_width = get_width,
    .get_height = get_height,
    .get_stride = get_stride,
    .get_format = get_format,
    .begin_access = NULL,
    .end_access = NULL,
};




struct ws_image_buffer*
ws_image_buffer_from_png(
    const char* filename
) {
    struct ws_image_buffer* buff = ws_image_buffer_new();
    png_image img;
    memset(&img, 0, sizeof(img));
    img.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_file(&img, filename)) {
        ws_log(NULL, "Could not read file: '%s' error: %s.",
                filename, img.message);
        return NULL;
    }

    img.format = PNG_FORMAT_BGRA;
    buff->buffer = calloc(1, PNG_IMAGE_SIZE(img));
    buff->height = img.height;
    buff->width = img.width;
    buff->stride = PNG_IMAGE_ROW_STRIDE(img);

    if (!buff->buffer) {
        ws_log(NULL, "Could not allocate memory for image: '%s'", filename);
        png_image_free(&img);
        return NULL;
    }

    if (!png_image_finish_read(&img, NULL, buff->buffer, 0, NULL)) {
        ws_log(NULL, "Could not read file: '%s'.", filename);
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
    struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    free(buff->buffer);
    return true;
}

static int
cmp_buffer(
    struct ws_object const* obj1,
    struct ws_object const* obj2
) {
    struct ws_image_buffer* buff1 = (struct ws_image_buffer*)obj1;
    struct ws_image_buffer* buff2 = (struct ws_image_buffer*)obj2;
    int cmp = strcmp(buff1->path, buff2->path);
    return signum(cmp);
}

static void*
get_data(
    struct ws_buffer* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    return buff->buffer;
}

static int32_t
get_height(
    struct ws_buffer* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    return  buff->height;
}

static int32_t
get_width(
    struct ws_buffer* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    return buff->width;
}

static int32_t
get_stride(
    struct ws_buffer* self
) {
    struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    return buff->stride;
}

static uint32_t
get_format(
    struct ws_buffer* self
) {
    //struct ws_image_buffer* buff = (struct ws_image_buffer*)self;
    return 0; //!< @todo: unused (But maybe of use later)
}

