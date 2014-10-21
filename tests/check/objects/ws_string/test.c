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

static Suite*
string_suite(void)
{
    Suite* s    = suite_create("String");
    TCase* tc   = tcase_create("main case");

    suite_add_tcase(s, tc);
    // tcase_add_checked_fixture(tc, setup, cleanup); // Not used yet

    tcase_add_test(tc, test_string_init);
    tcase_add_test(tc, test_string_empty_len);
    tcase_add_test(tc, test_string_cat_empty);

    return s;
}

WS_TESTS_CHECK_MAIN(string_suite);

/**
 * @}
 */

/**
 * @}
 */
