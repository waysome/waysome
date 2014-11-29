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

#include <check.h>
#include <stdlib.h>

#include "objects/object.h"
#include "util/string.h"
#include "values/int.h"
#include "values/string.h"

#define TEST_INT 1
#define TEST_CHR 'a'
#define TEST_STR "teststring"

struct ws_test_object {
    struct ws_object obj;

    int     int_attribute;
    char    char_attribute;

    char*   string_attribute;
    struct ws_object* object_attribute;
    /* more types here */
};

struct ws_object_attribute const WS_OBJECT_ATTRS_TEST_OBJ[] = {
    {
        .name = "int",
        .offset_in_struct = offsetof(struct ws_test_object, int_attribute),
        .type = WS_OBJ_ATTR_TYPE_INT32,
    },
    {
        .name = "char",
        .offset_in_struct = offsetof(struct ws_test_object, char_attribute),
        .type = WS_OBJ_ATTR_TYPE_CHAR,
    },
    {
        .name = "string",
        .offset_in_struct = offsetof(struct ws_test_object, string_attribute),
        .type = WS_OBJ_ATTR_TYPE_STRING,
    },
    {
        .name = "object",
        .offset_in_struct = offsetof(struct ws_test_object, object_attribute),
        .type = WS_OBJ_ATTR_TYPE_OBJ,
    },
    {
        .name = NULL,
        .offset_in_struct = 0,
        .type = 0
    },
};

ws_object_type_id WS_OBJECT_TYPE_ID_TESTOBJ = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_test_object",

    .deinit_callback    = NULL,
    .hash_callback      = NULL,
    .cmp_callback       = NULL,
    .attribute_table = WS_OBJECT_ATTRS_TEST_OBJ,
};

static struct ws_test_object* ws_test_object_new(void)
{
    struct ws_test_object* t = calloc(1, sizeof(*t));

    if (t) {
        ws_object_init(&t->obj);

        t->obj.id = &WS_OBJECT_TYPE_ID_TESTOBJ;
        t->obj.settings |= WS_OBJECT_HEAPALLOCED;

        t->int_attribute        = TEST_INT;
        t->char_attribute       = TEST_CHR;
        t->string_attribute     = TEST_STR;

        t->object_attribute     = ws_object_new_raw();
        if (!t->object_attribute) {
            ws_object_unref(&t->obj);
            return NULL;
        }
    }

    return t;
}

/*
 *
 * tests
 *
 */

START_TEST (test_object_attribute_type) {
    struct ws_test_object* to = ws_test_object_new();
    if (!to) { // If we fail to alloc, fail here
        ck_assert(0 != 0);
    }

    enum ws_object_attribute_type type;

    type = ws_object_attr_type(&to->obj, "int");
    ck_assert(WS_OBJ_ATTR_TYPE_INT32 == type || WS_OBJ_ATTR_TYPE_INT64 == type);

    type = ws_object_attr_type(&to->obj, "char");
    ck_assert(WS_OBJ_ATTR_TYPE_CHAR == type);

    type = ws_object_attr_type(&to->obj, "string");
    ck_assert(WS_OBJ_ATTR_TYPE_STRING == type);

    type = ws_object_attr_type(&to->obj, "object");
    ck_assert(WS_OBJ_ATTR_TYPE_OBJ == type);

    ws_object_unref(&to->obj);
}
END_TEST

START_TEST (test_object_attribute_read) {
    struct ws_test_object* to = ws_test_object_new();
    if (!to) { // If we fail to alloc, fail here
        ck_assert(0 != 0);
    }

    struct ws_value* v = NULL;
    int r = 0;

    v = calloc(1, sizeof(struct ws_value_int));
    ws_value_int_init((struct ws_value_int*) v);
    r = ws_object_attr_read(&to->obj, "int", v);
    ck_assert(r == 0);
    ck_assert(v != NULL);
    ck_assert(TEST_INT == ws_value_int_get((struct ws_value_int*) v));
    free(v);

//    v = (struct ws_value*) ws_value_string_new();
//    r = ws_object_attr_read(&to->obj, "string", v);
//    ck_assert(r == 0);
//    ck_assert(v != NULL);
//    struct ws_string* s = ws_value_string_get_str((struct ws_value_string*) v);
//    ck_assert(s != NULL);
//    char* raw = ws_string_raw(s);
//    ck_assert(raw != NULL);
//    ck_assert(ws_streq(TEST_STR, raw));
//    free(v);

    ws_object_unref(&to->obj);
}
END_TEST
