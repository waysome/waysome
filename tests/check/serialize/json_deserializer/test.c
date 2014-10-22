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
 * @addtogroup tests_objects "Testing: Serializer"
 *
 * @{
 */

/**
 * @addtogroup tests_objects "Testing: Serializer: JSON Deserializer"
 *
 * @{
 */

#include <check.h>
#include "tests.h"

#include "serialize/deserializer.h"
#include "serialize/json/deserializer.h"

/*
 *
 * Test cases
 *
 */

START_TEST (test_json_deserializer_setup) {
    struct ws_deserializer* d = ws_serializer_json_deserializer_new();

    ck_assert(d != NULL);
    ck_assert(d->deserialize != NULL);

    ws_deserializer_deinit(d);
    free(d);
}
END_TEST

/*
 *
 * setup/teardown helpers
 *
 */

struct ws_deserializer* d = NULL;
struct ws_message* messagebuf;

void
setup(void) {
    d = ws_serializer_json_deserializer_new();
}

void
teardown(void) {
    ws_deserializer_deinit(d);
    free(d);
    d = NULL;
}

/*
 *
 * testing with setup/teardown helper
 *
 */

START_TEST (test_json_deserializer_minimal) {
    char const* buf = "";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert(s == 0);
    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_json_object) {
    char const* buf = "{}";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

/*
 *
 * main()
 *
 */

static Suite*
json_deserializer_suite(void)
{
    Suite* s    = suite_create("Objects");
    TCase* tc   = tcase_create("main case");
    TCase* tcx  = tcase_create("main case with buffers");

    suite_add_tcase(s, tc);
    suite_add_tcase(s, tcx);
    tcase_add_checked_fixture(tcx, setup, teardown);

    tcase_add_test(tc, test_json_deserializer_setup);

    tcase_add_test(tcx, test_json_deserializer_minimal);
    tcase_add_test(tcx, test_json_deserializer_json_object);

    return s;
}

WS_TESTS_CHECK_MAIN(json_deserializer_suite);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

