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
#include <stdint.h>
#include <string.h>

#include "values/set.h"
#include "values/union.h"
#include "values/value_type.h"
#include "util/string.h"

int
ws_value_union_init_from_val(
    union ws_value_union* dest,
    struct ws_value* src
) {
    ws_value_deinit(&dest->value);

    switch(src->type) {
    default:
    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
        break;

    case WS_VALUE_TYPE_NIL:
        ws_value_nil_init(&dest->nil);
        return 0;

    case WS_VALUE_TYPE_BOOL:
        ws_value_bool_init(&dest->bool_);
        {
            bool buf = ws_value_bool_get((struct ws_value_bool*) src);
            return ws_value_bool_set(&dest->bool_, buf);
        }

    case WS_VALUE_TYPE_INT:
        ws_value_int_init(&dest->int_);
        {
            intmax_t buf = ws_value_int_get((struct ws_value_int*) src);
            return ws_value_int_set(&dest->int_, buf);
        }

    case WS_VALUE_TYPE_STRING:
        ws_value_string_init(&dest->string);
        {
            ws_string_set_from_str(dest->string.str,
                                    ((struct ws_value_string*)src)->str);
            return 0;
        }

    case WS_VALUE_TYPE_OBJECT_ID:
        ws_value_object_id_init(&dest->object_id);
        {
            struct ws_object* buf;
            buf = ws_value_object_id_get((struct ws_value_object_id*) src);
            if (!buf) {
                return -EINVAL;
            }
            ws_value_object_id_set(&dest->object_id, buf);
            ws_object_unref(buf);
            return 0;
        }

    case WS_VALUE_TYPE_SET:
        dest->set.set = ws_value_set_get((struct ws_value_set*) src);
        return 0;

    }
    return -EINVAL;
}


int
ws_value_union_reinit(
    union ws_value_union* self,
    enum ws_value_type type
) {
    ws_value_deinit(&self->value);

    switch(type) {
    default:
    case WS_VALUE_TYPE_NONE:
    case WS_VALUE_TYPE_VALUE:
        return -EINVAL;

    case WS_VALUE_TYPE_NIL:
        ws_value_nil_init(&self->nil);
        break;

    case WS_VALUE_TYPE_BOOL:
        ws_value_bool_init(&self->bool_);
        break;

    case WS_VALUE_TYPE_INT:
        ws_value_int_init(&self->int_);
        break;

    case WS_VALUE_TYPE_STRING:
        ws_value_string_init(&self->string);
        break;

    case WS_VALUE_TYPE_OBJECT_ID:
        ws_value_object_id_init(&self->object_id);
        break;

    case WS_VALUE_TYPE_SET:
        return ws_value_set_init(&self->set);
    }
    return 0;
}

char*
ws_value_union_tostr(
    union ws_value_union* self
) {
    char* res = NULL;

    switch (ws_value_get_type(&self->value)) {
    case WS_VALUE_TYPE_NONE:
        {
            static const char* none = "<none>";
            res = malloc(strlen(none) + 1);
            if (!res) {
                return NULL;
            }
            res[strlen(none)] = 0;
            memcpy(res, none, strlen(none));
        }
        break;

    case WS_VALUE_TYPE_VALUE:
        {
            static const char* value = "<value>";
            res = malloc(strlen(value) + 1);
            if (!res) {
                return NULL;
            }
            res[strlen(value)] = 0;
            memcpy(res, value, strlen(value));
        }
        break;

    case WS_VALUE_TYPE_NIL:
        {
            static const char* nil = "<nil>";
            res = malloc(strlen(nil) + 1);
            if (!res) {
                return NULL;
            }
            res[strlen(nil)] = 0;
            memcpy(res, nil, strlen(nil));
        }
        break;

    case WS_VALUE_TYPE_BOOL:
        {
            bool b = ws_value_bool_get(&self->bool_);
            char* bstr = (b ? "true" : "false");
            res = malloc(strlen(bstr) + 1);
            if (!res) {
                return NULL;
            }
            res[strlen(bstr)] = 0;
            memcpy(res, bstr, strlen(bstr));
        }
        break;

    case WS_VALUE_TYPE_INT:
            {
                size_t strl = strlen(STR_OF(INTMAX_MAX));
                intmax_t j  = ws_value_int_get(&self->int_);
                res = calloc(1, strl + 1);
                if (!res) {
                    return NULL;
                }
                snprintf(res, strl, "%ld", j);
            }
            break;

    case WS_VALUE_TYPE_STRING:
            {
                struct ws_string* sstr  = ws_value_string_get(&self->string);
                char* s = ws_string_raw(sstr);
                size_t len = strlen(s) + 1;
                res = malloc(len);
                if (res) {
                    snprintf(res, len, "%s", s);
                }
                free(s);
            }
            break;

    case WS_VALUE_TYPE_OBJECT_ID:
            {
                struct ws_object* o = ws_value_object_id_get(&self->object_id);
                uintmax_t id        = ws_object_uuid(o);
                size_t strl         = strlen(STR_OF(INTMAX_MAX)) + 3;

                res = calloc(1, strl + 1);
                if (!res) {
                    return NULL;
                }

                snprintf(res, strl, "id:%ld", id);
            }
            break;

    case WS_VALUE_TYPE_SET:
            {
                size_t card = ws_value_set_cardinality(&self->set);
                char* fmt   = "{ ... }:%zi";
                size_t len  = strlen(STR_OF(SIZE_MAX)) + strlen(fmt) - 3;

                res = calloc(1, len + 1);
                if (!res) {
                    return NULL;
                }

                snprintf(res, len, fmt, card);
            }
            break;

    default:
            break;
    }

    return res;
}

