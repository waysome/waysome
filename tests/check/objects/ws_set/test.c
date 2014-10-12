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
 * @addtogroup tests_objects_set "Testing: Set"
 *
 * @{
 */

#include <check.h>
#include <stdlib.h>

#include "tests.h"
#include "objects/object.h"
#include "objects/set.h"

static struct ws_set* set = NULL;

static const int N_TEST_OBJS = 15;
static struct ws_object* TEST_OBJS[N_TEST_OBJS] = { 0 };

/*
 *
 * Setup/Teardown functions
 *
 */

static void
test_set_setup(void)
{
    ck_assert(set == NULL);
    set = ws_set_new();
    ck_assert(set != NULL);
}

static void
test_set_setup_objs(void)
{
    test_set_setup();

    for (int i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(TEST_OBJS[i] == NULL);

        TEST_OBJS[i] = ws_object_new_raw();

        ck_assert(TEST_OBJS[i] != NULL);
    }
}

static void
test_set_teardown(void)
{
    ck_assert(set != NULL);
    ck_assert(true == ws_object_deinit(&set->obj));
    free(set);
    set = NULL;
    ck_assert(set == NULL);
}

static void
test_set_teardown_objs(void)
{
    test_set_teardown(); // So the set deallocates the own objects

    // and now, we can clean up

    int i;

    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(TEST_OBJS[i] != NULL);
        ck_assert(true == ws_object_deinit(TEST_OBJS[i]));

        free(TEST_OBJS[i]);

        TEST_OBJS[i] = NULL;
    }
}

/*
 *
 * Tests
 *
 */

START_TEST (test_set_init) {
    ck_assert(0 == ws_set_init(set));
}
END_TEST

START_TEST (test_set_init_deinit) {
    ck_assert(0 == ws_set_init(set));
    ck_assert(true == ws_object_deinit((struct ws_object*) set));
    ck_assert(0 == ws_set_init(set));
    // reinitialize here, so we do not segfault in the teardown function
}
END_TEST

/*
 *
 * Suite
 *
 */

static Suite*
set_suite(void)
{
    Suite* s    = suite_create("Set");
    TCase* tc   = tcase_create("main case");
    TCase* tce  = tcase_create("Elements case");

    suite_add_tcase(s, tc);
    tcase_add_checked_fixture(tc, test_set_setup, test_set_teardown);

    tcase_add_test(tc, test_set_init);
    tcase_add_test(tc, test_set_init_deinit);

    suite_add_tcase(s, tce);
    tcase_add_checked_fixture(tce, test_set_setup_objs, test_set_teardown_objs);

    return s;
}

WS_TESTS_CHECK_MAIN(set_suite);

/**
 * @}
 */

/**
 * @}
 */

