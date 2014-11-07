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

    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_json_serializer_setup);

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

