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

/**
 * @addtogroup tests "Testing"
 *
 * @{
 */

/**
 * @addtogroup tests_objects "Testing: Objects"
 *
 * @{
 */

#include <errno.h>
#include <check.h>
#include <stdbool.h>

#include "tests.h"
#include "objects/ws_object/attribute_test.c"

#include "objects/object.h"

START_TEST (test_object_init) {
    struct ws_object o;
    memset(&o, 0, sizeof(o));

    ck_assert(ws_object_init(&o));
    ws_object_deinit(&o);
}
END_TEST

START_TEST (test_object_alloc) {
    ck_assert(ws_object_new(1) == NULL);

    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(o != NULL);

    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_type_id) {
    struct ws_object* o = ws_object_new(sizeof(*o));

    ck_assert(&WS_OBJECT_TYPE_ID_OBJECT == ws_object_get_type_id(o));

    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_settings) {
    struct ws_object os;
    memset(&os, 0, sizeof(os));
    ws_object_init(&os);
    ck_assert(WS_OBJ_NO_SETTINGS == ws_object_get_settings(&os));
    ws_object_deinit(&os);

    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(WS_OBJECT_HEAPALLOCED == ws_object_get_settings(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_settings_set) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(WS_OBJECT_HEAPALLOCED == ws_object_get_settings(o));

    ws_object_set_settings(o, WS_OBJ_NO_SETTINGS);
    ck_assert(WS_OBJ_NO_SETTINGS == ws_object_get_settings(o));

    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_getref) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ws_object_getref(o);
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_unref) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ws_object_getref(o);
    ws_object_unref(o);
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_cb_hash) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(-ENOTSUP == ws_object_hash(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_lock_read) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(true == ws_object_lock_read(o));
    ck_assert(true == ws_object_unlock(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_lock_write) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(true == ws_object_lock_write(o));
    ck_assert(true == ws_object_unlock(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_lock_try_read) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(0 == ws_object_lock_try_read(o));
    ck_assert(true == ws_object_unlock(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_lock_try_write) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(0 == ws_object_lock_try_write(o));
    ck_assert(true == ws_object_unlock(o));
    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_cmp) {
    struct ws_object* o = ws_object_new(sizeof(*o));

    ck_assert(ws_object_cmp(NULL, NULL) == 0);
    ck_assert(ws_object_cmp(o, NULL) == -1);
    ck_assert(ws_object_cmp(NULL, o) == 1);

    int t = ws_object_cmp(o, o);
    ck_assert(t != -1);
    ck_assert(t == 0);
    ck_assert(t != 1);

    ws_object_deinit(o);
}
END_TEST

START_TEST (test_object_uuid) {
    struct ws_object* o = ws_object_new(sizeof(*o));
    ck_assert(o);

    size_t id = ws_object_uuid(o);

    ck_assert(id == ws_object_uuid(o));
    ck_assert(id == ws_object_uuid(o));
    ck_assert(id == ws_object_uuid(o));
    ck_assert(id == ws_object_uuid(o));

    ws_object_unref(o);
}
END_TEST

static Suite*
objects_suite(void)
{
    Suite* s    = suite_create("Objects");
    TCase* tc   = tcase_create("main case");
    TCase* tca  = tcase_create("Attribute case");

    suite_add_tcase(s, tc);
    suite_add_tcase(s, tca);
    // tcase_add_checked_fixture(tc, setup, cleanup); // Not used yet

    tcase_add_test(tc, test_object_init);
    tcase_add_test(tc, test_object_alloc);
    tcase_add_test(tc, test_object_type_id);
    tcase_add_test(tc, test_object_settings);
    tcase_add_test(tc, test_object_settings_set);
    tcase_add_test(tc, test_object_getref);
    tcase_add_test(tc, test_object_unref);
    tcase_add_test(tc, test_object_cb_hash);
    tcase_add_test(tc, test_object_lock_read);
    tcase_add_test(tc, test_object_lock_write);
    tcase_add_test(tc, test_object_lock_try_read);
    tcase_add_test(tc, test_object_lock_try_write);
    tcase_add_test(tc, test_object_cmp);
    tcase_add_test(tc, test_object_uuid);

    tcase_add_test(tca, test_object_attribute_type);
    tcase_add_test(tca, test_object_attribute_read);

    return s;
}

WS_TESTS_CHECK_MAIN(objects_suite);

/**
 * @}
 */

/**
 * @}
 */
