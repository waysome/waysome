#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <xf86drmMode.h>

#include "logger/module.h"
#include "background_surface.h"

struct ws_image_buffer*
ws_background_service_load_image(
    const char* filename
) {
    struct ws_image_buffer* buff = calloc(1, sizeof(*buff));
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

