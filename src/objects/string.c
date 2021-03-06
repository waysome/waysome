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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ustring.h>

#include "objects/object.h"
#include "objects/string.h"
#include "util/condition.h"

/*
 *
 *
 * forward declarations
 *
 *
 */

static bool
deinit_callback(
    struct ws_object* const
);

/*
 *
 *
 * v-table
 *
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_STRING = {
    .supertype = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr = "ws_string",

    .deinit_callback = deinit_callback,
    .hash_callback = NULL,
    .cmp_callback = (int (*) (struct ws_object const*, struct ws_object const*))
                    ws_string_cmp,
    .uuid_callback = NULL,

    .attribute_table = NULL,
    .function_table = NULL,
};

bool
ws_string_init(
    struct ws_string* self
) {
    if (!self) {
        return false;
    }

    self->str = calloc(1, sizeof(*self->str)); //initialize as empty string
    if (unlikely(!self->str)) {
        return false;
    }

    ws_object_init(&self->obj);
    self->obj.id = &WS_OBJECT_TYPE_ID_STRING;

    return true;
}

bool
ws_string_set_from_str(
    struct ws_string* self,
    struct ws_string* other
) {
    if (unlikely(!self || !other)) {
        return false;
    }

    ws_object_lock_write(&self->obj);
    ws_object_lock_read(&other->obj);

    size_t charcount = u_strlen(other->str);

    UChar* temp = realloc(self->str, (charcount + 1) * sizeof(*self->str));
    if (unlikely(!temp)) {
        ws_object_unlock(&other->obj);
        ws_object_unlock(&self->obj);
        return false;
    }
    self->str = temp;

    u_strcpy(self->str, other->str);

    ws_object_unlock(&other->obj);
    ws_object_unlock(&self->obj);

    return true;
}

struct ws_string*
ws_string_new(void)
{
    struct ws_string* wss = calloc(1, sizeof(*wss));

    if (likely(wss)) {
        ws_string_init(wss);
        wss->obj.settings |= WS_OBJECT_HEAPALLOCED;
    }

    return wss;
}

size_t
ws_string_len(
    struct ws_string* self
){
    if (likely(self)) {
        size_t len;
        ws_object_lock_read(&self->obj);
        len = u_strlen(self->str);
        ws_object_unlock(&self->obj);
        return len;
    }

    return 0;
}

struct ws_string*
ws_string_cat(
    struct ws_string* self,
    struct ws_string* other
){
    ws_object_lock_write(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    size_t new_len = u_strlen(self->str) + u_strlen(other->str) + 1;
    UChar* temp = realloc(self->str, new_len * sizeof(*self->str));
    if (unlikely(!temp)) {
        ws_object_unlock(&other->obj);
        ws_object_unlock(&self->obj);
        return NULL;
    }
    self->str = temp;

    u_strcat(self->str, other->str);

    ws_object_unlock(&other->obj);
    ws_object_unlock(&self->obj);

    return self;
}

struct ws_string*
ws_string_dupl(
    struct ws_string* self
){
    struct ws_string* nstr = ws_string_new();
    if (unlikely(!nstr || !ws_string_set_from_str(nstr, self))) {
        return NULL;
    }

    return nstr;
}

int
ws_string_cmp(
    struct ws_string* self,
    struct ws_string* other
){
    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    int res = u_strcmp(self->str, other->str);

    ws_object_unlock(&other->obj);
    ws_object_unlock(&self->obj);

    return res;
}

int
ws_string_ncmp(
    struct ws_string* self,
    struct ws_string* other,
    size_t offset,
    size_t n
){
    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    int res = u_strncmp(self->str + offset, other->str, n);

    ws_object_unlock(&self->obj);
    ws_object_unlock(&other->obj);

    return res;
}

bool
ws_string_substr(
    struct ws_string* self,
    struct ws_string* other
){
    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    UChar* res = u_strstr(self->str, other->str);

    ws_object_unlock(&self->obj);
    ws_object_unlock(&other->obj);

    return !!res;
}

char*
ws_string_raw(
    struct ws_string* self
){
    ws_object_lock_read(&self->obj);

    int32_t dest_len;
    UErrorCode err = U_ZERO_ERROR;
    size_t charcount = u_strlen(self->str);

    u_strToUTF8(NULL, 0, &dest_len, self->str, charcount, &err);
    if ((err != U_BUFFER_OVERFLOW_ERROR) && U_FAILURE(err)) {
        return NULL;
    }

    char* output = calloc(dest_len + 1, sizeof(*output)); // +1 => Nullbyte
    if (unlikely(!output)) {
        return NULL;
    }

    err = U_ZERO_ERROR;
    output = u_strToUTF8(output, charcount, &dest_len, self->str,
                         charcount, &err);

    ws_object_unlock(&self->obj);

    if (U_FAILURE(err) || (dest_len <= 0)) {
        free(output);
        return NULL;
    }

    return output;
}

int
ws_string_set_from_raw(
    struct ws_string* self,
    const char* raw
){
    if (unlikely(!self)) {
        return -EINVAL;
    }

    UErrorCode err = U_ZERO_ERROR;

    // get the length of the buffer to allocate
    int32_t len;
    u_strFromUTF8(NULL, 0, &len, raw, -1, &err);
    if ((err != U_BUFFER_OVERFLOW_ERROR) && U_FAILURE(err)) {
        return -ENOMEM; //!< @todo is it actually -ENOMEM?
    }

    // allocate buffer
    ++len;
    UChar* conv_raw = calloc(1, sizeof(*conv_raw) * len);
    if (unlikely(!conv_raw)) {
        return -ENOMEM;
    }

    // convert string (write into buffer)
    err = U_ZERO_ERROR;
    conv_raw = u_strFromUTF8(conv_raw, len, NULL, raw, -1, &err);
    if (U_FAILURE(err)) {
        return -ENOMEM; //!< @todo is it actually -ENOMEM?
    }

    ws_object_lock_write(&self->obj);

    free(self->str);
    self->str = conv_raw;

    ws_object_unlock(&self->obj);

    return 0;
}

/*
 *
 * Static Function Implementations
 *
 */
static bool
deinit_callback(
    struct ws_object* const self
) {
    if (likely(self)) {
        free(((struct ws_string*) self)->str);
        return true;
    }
    return false;
}
