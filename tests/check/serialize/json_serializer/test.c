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
#include <inttypes.h>
#include <check.h>
#include "tests.h"

#include "serialize/serializer.h"
#include "serialize/json/serializer.h"
#include "serialize/json/keys.h"
#include "objects/message/error_reply.h"
#include "objects/message/message.h"
#include "objects/message/event.h"
#include "objects/message/transaction.h"
#include "objects/message/value_reply.h"
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

static struct ws_event*
mkevent_with_ctx(
    char* ev_name,
    struct ws_value* ctx
) {
    struct ws_event* ev = calloc(1, sizeof(*ev));
    ck_assert(ev);

    struct ws_string* name = ws_string_new();
    ck_assert(name);
    ws_string_set_from_raw(name, ev_name);

    ck_assert(0 == ws_event_init(ev, name, ctx));

    return ev;
}

static struct ws_event*
mkevent(
    char* ev_name
) {
    struct ws_value_int* ctx = calloc(1, sizeof(*ctx));
    ck_assert(ctx);
    ws_value_int_init(ctx);
    ws_value_int_set(ctx, 1);

    return mkevent_with_ctx(ev_name, (struct ws_value*) ctx);
}

static struct ws_value_reply*
mk_value_reply(
    char* transaction_name,
    struct ws_value* v,
    size_t trans_id
) {
    struct ws_string* tname = ws_string_new();
    ck_assert(tname);

    ws_string_set_from_raw(tname, transaction_name);

    struct ws_transaction* t = ws_transaction_new(trans_id, tname, 0, NULL);
    ck_assert(t);

    struct ws_value_reply* vr = ws_value_reply_new(t, v);
    ck_assert(vr);

    return vr;
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

START_TEST (test_json_serializer_event) {
    struct ws_event* ev = mkevent("teststring");

    size_t nbuf = 1000; // 1000 bytes are enough, hopefully
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    ssize_t s = ws_serialize(ser, buf, nbuf, (struct ws_message*) ev);
    ck_assert(s > 0);

    const char* expected = "{\"event\":{\"context\":1,\"name\":\"teststring\"}}";
    ck_assert(0 == strcmp(expected, buf));

    ws_object_unref((struct ws_object*) ev);
    free(buf);
}
END_TEST

START_TEST (test_json_serializer_event_smallbuf) {
    struct ws_event* ev = mkevent("teststring");
    ssize_t s; // Number of written bytes
    size_t nbuf = 15; // 1000 bytes are enough, hopefully
    size_t inc = 10;
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    {
        s = ws_serialize(ser, buf, nbuf, (struct ws_message*) ev);
        ck_assert(s == (ssize_t) nbuf);

        const char* exp= "{\"event\":{\"cont";
        ck_assert(0 == strcmp(exp, buf));
    }

    char* tmp = realloc(buf, sizeof(*buf) * (nbuf + inc));
    ck_assert(tmp);
    buf = tmp;

    {
        s = ws_serialize(ser, buf + nbuf, inc, (struct ws_message*) ev);
        ck_assert(s == -EAGAIN);

        const char* exp = "{\"event\":{\"context\":1,\"na";
        ck_assert(0 == strncmp(exp, buf, strlen(exp)));
    }

    nbuf += inc;

    tmp = realloc(buf, sizeof(*buf) * (nbuf + inc));
    ck_assert(tmp);
    buf = tmp;

    {
        s = ws_serialize(ser, buf + nbuf, inc, (struct ws_message*) ev);
        ck_assert(s == -EAGAIN);

        const char* exp = "{\"event\":{\"context\":1,\"name\":\"tests";
        ck_assert(0 == strncmp(exp, buf, strlen(exp)));
    }

    nbuf += inc;

    tmp = realloc(buf, sizeof(*buf) * (nbuf + inc));
    ck_assert(tmp);
    buf = tmp;

    {
        s = ws_serialize(ser, buf + nbuf, inc, (struct ws_message*) ev);
        ck_assert(s == (ssize_t) inc);

        const char* exp = "{\"event\":{\"context\":1,\"name\":\"teststring\"}}";
        ck_assert(0 == strncmp(exp, buf, strlen(exp)));
    }

    ws_object_unref((struct ws_object*) ev);
    free(buf);
}
END_TEST

START_TEST (test_json_serializer_event_with_objid) {
    struct ws_value_object_id* ctx = calloc(1, sizeof(*ctx));
    ck_assert(ctx);

    struct ws_object* obj = ws_object_new_raw();
    ck_assert(obj);

    ws_value_object_id_init(ctx);
    ws_value_object_id_set(ctx, obj);

    struct ws_event* ev;
    ev = mkevent_with_ctx("teststring", (struct ws_value*) ctx);
    ssize_t s; // Number of written bytes
    size_t nbuf = 1000; // 1000 bytes are enough, hopefully
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    s = ws_serialize(ser, buf, nbuf, (struct ws_message*) ev);

    { // test the result
        char exp[1024];
        memset(exp, 0, 1024);
        const char* pref = "{\"event\":{\"context\":{\"object\":\"";
        const char* suff = "\"},\"name\":\"teststring\"}}";
        uintmax_t id = ws_object_uuid(obj);
        snprintf(exp, 1024, "%s%"PRIxMAX"%s", pref, id, suff);
        ck_assert(0 == strcmp(exp, buf));

        // we can now check the returned value
        ck_assert(s == (ssize_t) strlen(exp));
    }

    ws_object_unref((struct ws_object*) ev);
    free(buf);
}
END_TEST

START_TEST (test_json_serializer_value_reply) {
    size_t t_id = 13;
    struct ws_value_reply* vr = mk_value_reply("testtrans", NULL, t_id);

    ssize_t s; // Number of written bytes
    size_t nbuf = 1000; // 1000 bytes are enough, hopefully
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    s = ws_serialize(ser, buf, nbuf, (struct ws_message*) vr);

    ck_assert(s != 0);

    { // test the result
        char exp[1024];
        memset(exp, 0, 1024);
        const char* pref = "{\"value\":null,\""TRANSACTION_ID"\":";
        const char* suff = "}";
        snprintf(exp, 1024, "%s%zi%s", pref, t_id, suff);
        ck_assert(0 == strcmp(exp, buf));

        // we can now check the returned value
        ck_assert(s == (ssize_t) strlen(exp));
    }

    ws_object_unref((struct ws_object*) vr);
    free(buf);
}
END_TEST

START_TEST (test_json_serializer_value_reply_int) {
    size_t t_id = 42;
    int v_int = 1337; // Is hardcoded in expected value!!!

    struct ws_value_int* v = calloc(1, sizeof(*v));
    ck_assert(v);

    ws_value_int_init(v);
    ws_value_int_set(v, v_int);

    struct ws_value_reply* vr = mk_value_reply("testtrans",
                                               (struct ws_value*) v,
                                               t_id);

    ssize_t s; // Number of written bytes
    size_t nbuf = 1000; // 1000 bytes are enough, hopefully
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    s = ws_serialize(ser, buf, nbuf, (struct ws_message*) vr);

    ck_assert(s != 0);

    { // test the result
        char exp[1024];
        memset(exp, 0, 1024);
        const char* pref = "{\"value\":1337,\""TRANSACTION_ID"\":";
        const char* suff = "}";
        snprintf(exp, 1024, "%s%zi%s", pref, t_id, suff);
        ck_assert(0 == strcmp(exp, buf));

        // we can now check the returned value
        ck_assert(s == (ssize_t) strlen(exp));
    }

    ws_object_unref((struct ws_object*) vr);
    free(buf);
}
END_TEST

START_TEST (test_json_serializer_error_reply) {
    size_t t_id = 132;
    unsigned int code = 12345;
#define DESC "Hello"
#define CAUSE "world"

    struct ws_string* tname = ws_string_new();
    ck_assert(tname);

    ws_string_set_from_raw(tname, "testtransaction");
    struct ws_transaction* t = ws_transaction_new(t_id, tname, 0, NULL);
    ck_assert(t);

    struct ws_error_reply* er = ws_error_reply_new(t, code, DESC, CAUSE);
    ck_assert(er);

    ssize_t s; // Number of written bytes
    size_t nbuf = 1000; // 1000 bytes are enough, hopefully
    char* buf   = calloc(1, sizeof(*buf) * nbuf);
    ck_assert(buf);

    s = ws_serialize(ser, buf, nbuf, (struct ws_message*) er);

    ck_assert(s != 0);

    { // test the result
        char exp[1024];
        memset(exp, 0, 1024);
        const char* pref = "{\""ERROR_CODE"\":";
        const char* suff =  ",\""ERROR_DESC"\":\""DESC"\","
                            "\""ERROR_CAUSE"\":\""CAUSE"\"}";
        snprintf(exp, 1024, "%s%zi%s", pref, code, suff);
        ck_assert(0 == strcmp(exp, buf));

        // we can now check the returned value
        ck_assert(s == (ssize_t) strlen(exp));
    }

#undef DESC
#undef CAUSE

    ws_object_unref((struct ws_object*) er);
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
    tcase_add_test(tcx, test_json_serializer_event);
    tcase_add_test(tcx, test_json_serializer_event_smallbuf);
    tcase_add_test(tcx, test_json_serializer_event_with_objid);

    tcase_add_test(tcx, test_json_serializer_value_reply);
    tcase_add_test(tcx, test_json_serializer_value_reply_int);
    tcase_add_test(tcx, test_json_serializer_error_reply);

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

