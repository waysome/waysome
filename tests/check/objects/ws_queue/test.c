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
 * @addtogroup tests_queue "Testing: Queue"
 *
 * @{
 */

#include <errno.h>
#include <check.h>
#include <stdbool.h>

#include "objects/queue.h"
#include "tests.h"

START_TEST (test_queue_init) {
    struct ws_queue q;
    int res = ws_queue_init(&q);

    ck_assert(res == 0);
}
END_TEST

START_TEST (test_queue_new) {
    struct ws_queue* q = ws_queue_new();
    ck_assert(q);
    ws_object_unref(&q->obj);
}
END_TEST

START_TEST (test_queue_push) {
    struct ws_queue* q = ws_queue_new();
    ck_assert(q);

    struct ws_object* o1 = ws_object_new_raw();
    struct ws_object* o2 = ws_object_new_raw();
    struct ws_object* o3 = ws_object_new_raw();

    ck_assert(0 != ws_queue_push(q, NULL));
    ck_assert(0 != ws_queue_push(NULL, o1));
    ck_assert(0 != ws_queue_push(NULL, NULL));

    ck_assert(0 == ws_queue_push(q, o1));
    ck_assert(0 == ws_queue_push(q, o2));
    ck_assert(0 == ws_queue_push(q, o3));

    ws_object_unref(&q->obj);
}
END_TEST

static Suite*
queue_suite(void)
{
    Suite* s    = suite_create("Queue");
    TCase* tc   = tcase_create("main case");

    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_queue_init);
    tcase_add_test(tc, test_queue_new);
    tcase_add_test(tc, test_queue_push);
    return s;
}

WS_TESTS_CHECK_MAIN(queue_suite);

/**
 * @}
 */

/**
 * @}
 */
