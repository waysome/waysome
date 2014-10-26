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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ustring.h>

#include "objects/string.h"

#include "objects/object.h"
#include "objects/array.h"

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
    .dump_callback = NULL,
    .run_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = (int (*) (struct ws_object const*, struct ws_object const*))
                    ws_string_cmp,
    .uuid_callback = NULL,
};

bool
ws_string_init(
    struct ws_string* self
) {
    if (self) {
        self->charcount = 0; //initialize as empty string
        self->str = calloc(self->charcount + 1, sizeof(*self->str));

        if (!self->str) {
            return false;
        }

        ws_object_init(&self->obj);
        self->obj.id = &WS_OBJECT_TYPE_ID_STRING;

        self->is_utf8 = false;

        return true;
    }

    return false;
}

struct ws_string*
ws_string_new(void)
{
    struct ws_string* wss = calloc(1, sizeof(*wss));

    if (wss) {
        ws_string_init(wss);
        wss->obj.settings |= WS_OBJECT_HEAPALLOCED;
    }

    return wss;
}

size_t
ws_string_len(
    struct ws_string* self
){
    if (self) {
        size_t len;
        ws_object_lock_read(&self->obj);
        len = self->charcount;
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

    self->str = realloc(self->str, (self->charcount + other->charcount + 1)
                        * sizeof(*self->str));
    self->str = u_strcat(self->str, other->str);

    ws_object_unlock(&other->obj);
    ws_object_unlock(&self->obj);

    if (self->str) {
        return self;
    }

    return NULL;
}


struct ws_string*
ws_string_multicat(
    struct ws_string* self,
    struct ws_array* others
){
    if (!self || !others) {
        return NULL;
    }

    ws_object_lock_write(&self->obj); //!< @todo Thread-safeness!

    int len = 0;
    for (unsigned int i = 0; i < others->len; i++) {
        struct ws_string* temp = (struct ws_string*) ws_array_get_at(others, i);
        if (temp && temp->str) {
            len += temp->charcount;
        } else {
            ws_object_unlock(&self->obj);
            return NULL;
        }
    }

    self->str = realloc(self->str, (len + 1) * sizeof(*self->str));

/* This loop doesn't check for NULL in `others`, assuming that in the final
 * (threadsafe) implementation, the array will be locked during the process, so that
 * it was already completely checked at this point. Depending on the final locking
 * implementation, this might have to be adjusted later.
 */
    for (unsigned int i = 0; i < others->len; i++) {
        self->str = u_strcat(self->str,
                            ((struct ws_string*)ws_array_get_at(others, i))->str);

        if (!self) {
            ws_object_unlock(&self->obj);
            return NULL;
        }
    }

    ws_object_unlock(&self->obj);

    return self;
}


struct ws_string*
ws_string_dupl(
    struct ws_string* self
){
    struct ws_string* nstr = ws_string_new();

    if (nstr) {
        ws_object_lock_read(&self->obj);

        nstr->charcount = self->charcount;
        nstr->is_utf8 = self->is_utf8;
        nstr->str = realloc(nstr->str, (self->charcount + 1) * sizeof(*self->str));
        nstr->str = u_strcpy(nstr->str, self->str);

        ws_object_unlock(&self->obj);

        if (nstr->str) {
            return nstr;
        }
    }

    return NULL;
}

int
ws_string_cmp(
    struct ws_string* self,
    struct ws_string* other
){
    int res;

    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    res = u_strcmp(self->str, other->str);

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
    int res;

    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    res = u_strncmp(self->str + offset, other->str, n);

    ws_object_unlock(&self->obj);
    ws_object_unlock(&other->obj);

    return res;
}

bool
ws_string_substr(
    struct ws_string* self,
    struct ws_string* other
){
    UChar* res;
    ws_object_lock_read(&self->obj);
    ws_object_lock_read(&other->obj); //!< @todo Thread-safeness!

    res = u_strstr(self->str, other->str);

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

    (void) u_strToUTF8(NULL, 0, &dest_len, self->str, self->charcount, &err);
    if ((err != U_BUFFER_OVERFLOW_ERROR) && U_FAILURE(err)) {
        return NULL;
    }

    char* output;
    output = calloc(dest_len + 1, sizeof(*output)); // +1 => Nullbyte
    if (!output) {
        return NULL;
    }

    err = U_ZERO_ERROR;
    output = u_strToUTF8(output, self->charcount, &dest_len, self->str,
                         self->charcount, &err);

    ws_object_unlock(&self->obj);

    if (U_FAILURE(err) || (dest_len <= 0)) {
        free(output);
        return NULL;
    }

    return output;
}

void
ws_string_set_from_raw(
    struct ws_string* self,
    char* raw
){
    if (!self) {
        return;
    }

    UErrorCode err = U_ZERO_ERROR;

    int32_t len;
    (void) u_strFromUTF8(NULL, 0, &len, raw, -1, &err);
    if (U_FAILURE(err)) {
        return;
    }

    UChar* conv_raw = calloc(len, sizeof(*conv_raw));
    if (!conv_raw) {
        return;
     }

    conv_raw = u_strFromUTF8(conv_raw, len, NULL, raw, -1, &err);
    if (U_FAILURE(err)) {
        return;
    }

    ws_object_lock_write(&self->obj);

    free(self->str);

    self->str = conv_raw;
    self->is_utf8 = true;
    self->charcount = len;

    ws_object_unlock(&self->obj);
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
    if (self) {
        free(((struct ws_string*) self)->str);
        return true;
    }
    return false;
}
