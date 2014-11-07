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

#include <errno.h>
#include <check.h>
#include "tests.h"

#include "serialize/serializer.h"
#include "serialize/json/serializer.h"
#include "serialize/json/keys.h"
#include "objects/message/message.h"
#include "objects/message/transaction.h"
#include "command/statement.h"

#include "values/int.h"
#include "values/string.h"

/*
 *
 * Setup and teardown functions, including variables
 *
 */

static struct ws_serializer*    ser = NULL;

static void
setup(void)
{
    ser = ws_serializer_json_serializer_new();
    ck_assert(ser);
}

static void
teardown(void)
{
    ws_serializer_deinit(ser);
    ser = NULL;
}

/*
 *
 * Test cases
 *
 */

START_TEST (test_json_serializer_setup) {
    struct ws_serializer* d = ws_serializer_json_serializer_new();

    ck_assert(d != NULL);
    ck_assert(d->serialize != NULL);

    ws_serializer_deinit(d);
    free(d);
}
END_TEST

START_TEST (test_json_serializer_message) {
    struct ws_message* msg = calloc(1, sizeof(*msg));
    ck_assert(msg);
    ck_assert(0 == ws_message_init(msg, 0));

    size_t nbuf = 100; // 100 bytes are enough
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    ssize_t s = ws_serialize(ser, buf, nbuf, msg);
    ck_assert(s == -EINVAL);

    free(buf);
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
    TCase* tcx  = tcase_create("serializing case");

    suite_add_tcase(s, tc);
    suite_add_tcase(s, tcx);
    tcase_add_checked_fixture(tcx, setup, teardown);

    tcase_add_test(tc, test_json_serializer_setup);

    tcase_add_test(tcx, test_json_serializer_message);

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

