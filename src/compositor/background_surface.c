#include <png.h>
#include <string.h>
#include <stdlib.h>

#include "logger/module.h"
#include "background_surface.h"

char*
ws_background_service_load_image(
    const char* filename
) {
    char* ret;
    png_image img;
    memset(&img, 0, sizeof(img));

    if (!png_image_begin_read_from_file(&img, filename)) {
        ws_log(NULL, "Could not read file: '%s'.", filename);
        return NULL;
    }
    ret = calloc(1, img.width * img.height * sizeof(png_byte) * 4);

    if (!ret) {
        ws_log(NULL, "Could not allocate memory for image: '%s'", filename);
        png_image_free(&img);
        return NULL;
    }

    if (!png_image_finish_read(&img, NULL, ret, 0, NULL)) {
        ws_log(NULL, "Could not read file: '%s'.", filename);
        return NULL;
    }
    return ret;
}

