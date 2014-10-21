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
 * @addtogroup tests_objects "Testing: String"
 *
 * @{
 */

#include <check.h>
#include <stdlib.h>

#include "tests.h"
#include "objects/string.h"

START_TEST (test_string_init) {
    struct ws_string ss;
    memset(&ss, 0, sizeof(ss));

    ck_assert(true == ws_string_init(&ss));

    struct ws_string* sh = NULL;
    ck_assert(false == ws_string_init(sh));
    sh = calloc(1, sizeof(*sh));
    ck_assert(true == ws_string_init(sh));

    ws_object_deinit(&sh->obj);
    free(sh);
    sh = NULL;

    sh = ws_string_new();
    ck_assert(NULL != sh);
    ws_object_deinit(&sh->obj);
    free(sh);
}
END_TEST

START_TEST (test_string_empty_len) {
    struct ws_string* s = ws_string_new();

    ck_assert(s != NULL);
    ck_assert(0 == ws_string_len(s));

    ws_object_deinit(&s->obj);
    free(s);
}
END_TEST

START_TEST (test_string_cat_empty) {
    struct ws_string* sa = ws_string_new();
    struct ws_string* sb = ws_string_new();

    ck_assert(sa != NULL);
    ck_assert(sb != NULL);

    ck_assert(0 == ws_string_len(sa));
    ck_assert(0 == ws_string_len(sb));

    ck_assert(sa == ws_string_cat(sa, sb));
    ck_assert(0 == ws_string_len(sa));

    ws_object_deinit(&sa->obj);
    ws_object_deinit(&sb->obj);
    free(sa);
    free(sb);
}
END_TEST

//!< @todo implement, ws_array is untested by now
//START_TEST (test_string_multicat_empty) {
//}
//END_TEST

START_TEST (test_string_empty_dupl) {
    struct ws_string* s = ws_string_new();
    struct ws_string* d = NULL;

    ck_assert(0 == ws_string_len(s));
    ck_assert(s != NULL);
    ck_assert(d == NULL);

    d = ws_string_dupl(s);
    ck_assert(d != NULL);

    ck_assert(0 == ws_string_len(s));
    ck_assert(0 == ws_string_len(d));

    ws_object_deinit(&s->obj);
    ws_object_deinit(&d->obj);
    free(s);
    free(d);
}
END_TEST

START_TEST (test_string_empty_cmp) {
    struct ws_string* sa = ws_string_new();
    struct ws_string* sb = ws_string_new();

    ck_assert(0 == ws_string_cmp(sa, sb));

    ws_object_deinit(&sa->obj);
    ws_object_deinit(&sb->obj);
    free(sa);
    free(sb);
}
END_TEST

START_TEST (test_string_empty_raw) {
    struct ws_string* s = ws_string_new();

    ck_assert(NULL == ws_string_raw(s));

    ws_object_deinit(&s->obj);
    free(s);
}
END_TEST

START_TEST (test_string_from_raw_literal) {
    struct ws_string* s = ws_string_new();
    char* literal = "helloworld";

    ws_string_set_from_raw(s, literal);

    char* buf = ws_string_raw(s);

    ck_assert(0 == strcmp(buf, literal));

    ws_object_deinit(&s->obj);
    free(s);
}
END_TEST

START_TEST (test_string_from_raw_heapbuf) {
    struct ws_string* s = ws_string_new();
    char* literal = "helloworld";
    char* hbuf = strdup(literal);

    ws_string_set_from_raw(s, hbuf);

    char* buf = ws_string_raw(s);

    ck_assert(0 == strcmp(buf, hbuf));

    ws_object_deinit(&s->obj);
    free(s);
    free(hbuf);
}
END_TEST

/*
 *
 *
 * Tests for multi string testing
 *
 *
 */

static struct ws_string* string_a;
static struct ws_string* string_b;

static void
setup(void) {
    string_a = ws_string_new();
    ws_string_set_from_raw(string_a, "Hello, World!");
    string_b = ws_string_new();
    ws_string_set_from_raw(string_b, "Hello");
}

static void
teardown(void) {
    ws_object_unref(&string_a->obj);
    ws_object_unref(&string_b->obj);
}

START_TEST (test_string_ncmp) {
    const size_t offset = 0;
    const size_t nchars = 3;
    ck_assert(0 == ws_string_ncmp(string_a, string_b, offset, nchars));
    ck_assert(0 != ws_string_ncmp(string_a, string_b, offset, 6));
    ck_assert(0 != ws_string_ncmp(string_a, string_b, offset, 20));
}
END_TEST

static Suite*
string_suite(void)
{
    Suite* s    = suite_create("String");
    TCase* tc   = tcase_create("main case");
    TCase* tc_m = tcase_create("Multi string case");

    suite_add_tcase(s, tc);
    suite_add_tcase(s, tc_m);

    tcase_add_checked_fixture(tc, setup, teardown); // Not used yet
    tcase_add_checked_fixture(tc_m, setup, teardown); // Not used yet

    tcase_add_test(tc, test_string_init);
    tcase_add_test(tc, test_string_empty_len);
    tcase_add_test(tc, test_string_cat_empty);
    //tcase_add_test(test_string_multicat_empty);
    tcase_add_test(tc, test_string_empty_dupl);
    tcase_add_test(tc, test_string_empty_cmp);
    tcase_add_test(tc, test_string_empty_raw);
    tcase_add_test(tc, test_string_from_raw_literal);
    tcase_add_test(tc, test_string_from_raw_heapbuf);

    tcase_add_test(tc_m, test_string_ncmp);

    return s;
}

WS_TESTS_CHECK_MAIN(string_suite);

/**
 * @}
 */

/**
 * @}
 */
