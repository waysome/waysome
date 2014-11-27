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
#include "serialize/json/keys.h"
#include "objects/message/event.h"
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

START_TEST (test_json_deserializer_json_object_kv_null) {
    char const* buf = "{ \"\": null }";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_json_object_kv_bool) {
    char const* buf = "{ \"\": true }";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_json_object_kv_int) {
    char const* buf = "{ \"\": 1 }";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_json_object_kv_string) {
    char const* buf = "{ \"\": \"\" }";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_message_id) {
    char const* buf = "{ \"" UID "\": 42 }";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf == NULL);
}
END_TEST

START_TEST (test_json_deserializer_transaction_type) {
    char const* buf = "{ \"" TYPE "\": \"" TYPE_TRANSACTION "\"}";
    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);
    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_TRANSACTION);

    struct ws_transaction* t = (struct ws_transaction*) messagebuf; // cast

    ck_assert(t->name == NULL);
    ck_assert(t->flags == 0);
    ck_assert(t->cmds == NULL);
}
END_TEST

START_TEST (test_json_deserializer_transaction_valid_nocmds) {
    char const* buf =   "{ \"" TYPE "\": \"" TYPE_TRANSACTION "\","
                        " \"" UID "\": 1337, "
                        " \"" COMMANDS "\": [] }";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    // We have a transaction, but without commands. So this is not a
    // transaction.
    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_TRANSACTION);
    ck_assert(messagebuf->id == 1337);
    struct ws_transaction* t = (struct ws_transaction*) messagebuf;

    ck_assert(t->name == NULL);
    ck_assert(t->flags == 0);
    ck_assert(t->cmds == NULL);
}
END_TEST

START_TEST (test_json_deserializer_transaction_one_command) {
    char const* buf =   "{ \"" TYPE "\": \"" TYPE_TRANSACTION "\","
                        " \"" UID "\": 1337, "
                        " \"" COMMANDS "\": ["
                            "{ \"add\": [ 1, 2 ] }"
                        "] }";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    // We have a transaction, but without commands. So this is not a
    // transaction.
    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_TRANSACTION);
    ck_assert(messagebuf->id == 1337);
    struct ws_transaction* t = (struct ws_transaction*) messagebuf;

    ck_assert(t->name == NULL);
    ck_assert(t->flags == 0);
    ck_assert(t->cmds != NULL);
    ck_assert(t->cmds->num == 1);
    ck_assert(t->cmds->statements != NULL);
    ck_assert(t->cmds->statements[0].command->name != NULL);
    ck_assert(0 == strcmp(t->cmds->statements[0].command->name, "add"));
    ck_assert(t->cmds->statements[0].command->command_type == regular);
    ck_assert(t->cmds->statements[0].args.num == 2);
    ck_assert(t->cmds->statements[0].args.vals[0].arg.val != NULL);
    ck_assert(t->cmds->statements[0].args.vals[0].arg.val[0].type == WS_VALUE_TYPE_INT);

    struct ws_value* val_one = t->cmds->statements[0].args.vals[0].arg.val;
    struct ws_value* val_two = t->cmds->statements[0].args.vals[1].arg.val;

    struct ws_value_int* one = (struct ws_value_int*) val_one;
    struct ws_value_int* two = (struct ws_value_int*) val_two;

    ck_assert(one->value.type == WS_VALUE_TYPE_INT);
    ck_assert(one->i == 1);

    ck_assert(two->value.type == WS_VALUE_TYPE_INT);
    ck_assert(two->i == 2);
}
END_TEST

START_TEST (test_json_deserializer_transaction_commands) {
    char const* buf =   "{ \"" TYPE "\": \"" TYPE_TRANSACTION "\","
                        " \"" UID "\": 1337, "
                        " \"" COMMANDS "\": ["
                            "{ \"add\":     [ 42, 2 ] },"
                            "{ \"sub\":     [ 42, 4 ] },"
                            "{ \"mul\":     [ 42, 6 ] },"
                            "{ \"div\":     [ 42, 8 ] },"
                            "{ \"lnot\":    [ 42 ] },"
                            "{ \"land\":    [ 42, 9, \"string\" ] },"
                            "{ \"lnand\":   [ 42, 9, \"string\" ] },"
                            "{ \"lor\":     [ 42, 9, \"string\" ] },"
                            "{ \"lnor\":    [ 42, 9, \"string\" ] },"
                            "{ \"lxor\":    [ 42, 9, \"string\" ] }"
                        "] }";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    // We have a transaction, but without commands. So this is not a
    // transaction.
    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_TRANSACTION);
    ck_assert(messagebuf->id == 1337);
    struct ws_transaction* t = (struct ws_transaction*) messagebuf;

    ck_assert(t->name == NULL);
    ck_assert(t->flags == 0);
    ck_assert(t->cmds != NULL);
    ck_assert(t->cmds->num == 10);
    ck_assert(t->cmds->statements != NULL);
    ck_assert(t->cmds->statements[0].command->name != NULL);

    char* commands[] = {
        "add", "sub", "mul", "div", "lnot", "land", "lnand", "lor", "lnor",
        "lxor", NULL
    };
    for (int i = 0; i < 10; i++) {
        ck_assert(0 == strcmp(t->cmds->statements[i].command->name,
                  commands[i]));
        ck_assert(t->cmds->statements[i].command->command_type == regular);
        ck_assert(t->cmds->statements[i].args.num >= 1);
        ck_assert(t->cmds->statements[i].args.vals[0].arg.val != NULL);
        ck_assert(t->cmds->statements[i].args.vals[0].arg.val[0].type ==
                  WS_VALUE_TYPE_INT);

        struct ws_value* val_one = t->cmds->statements[i].args.vals[0].arg.val;
        struct ws_value_int* one = (struct ws_value_int*) val_one;
        ck_assert(one->value.type == WS_VALUE_TYPE_INT);
        ck_assert(one->i == 42);

    }

    for (int i = 5; i < 10; i++) {
        struct ws_value* val_nine;
        struct ws_value* val_str;

        val_nine = t->cmds->statements[i].args.vals[1].arg.val;
        val_str = t->cmds->statements[i].args.vals[2].arg.val;

        struct ws_value_int* nine = (struct ws_value_int*) val_nine;
        struct ws_value_string* s = (struct ws_value_string*) val_str;

        ck_assert(nine->value.type == WS_VALUE_TYPE_INT);
        ck_assert(nine->i == 9);

        ck_assert(s->val.type == WS_VALUE_TYPE_STRING);
        ck_assert(s->str->str != NULL);
        //!< @todo compare s->str->str with "string"
    }

}
END_TEST

START_TEST (test_json_deserializer_flags) {
    char const* buf =   "{ \"" TYPE "\": \"" TYPE_TRANSACTION "\","
                        " \"" UID "\": 1337, "
                        " \"" FLAGS "\": { "
                                            "\"" FLAG_EXEC      "\": true,"
                                            "\"" FLAG_REGISTER  "\": \"name\""
                                        "}"
                        "}";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    struct ws_transaction* t = (struct ws_transaction*) messagebuf; // cast

    ck_assert(t->flags ==
            (WS_TRANSACTION_FLAGS_EXEC | WS_TRANSACTION_FLAGS_REGISTER));

    { // test whether the name is set correctly
        struct ws_string* t_name = ws_string_new();
        ws_string_set_from_raw(t_name, "name");


        ck_assert(ws_string_cmp(t_name, t->name) == 0);

        ws_object_unref((struct ws_object*) t_name);
    }

    ck_assert(t->cmds == NULL);
}
END_TEST

START_TEST (test_json_deserializer_events) {
    char const* buf = "{ \"" EVENT_NAME "\": \"testname\" }";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_EVENT);
}
END_TEST

START_TEST (test_json_deserializer_events_with_type) {
    char const* buf = "{ \"" TYPE "\": \"" TYPE_EVENT "\" }";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_EVENT);
}
END_TEST

START_TEST (test_json_deserializer_events_with_everything) {
    char const* buf =   "{"
                        "\"" TYPE "\": \"" TYPE_EVENT "\","
                        "\"" EVENT_NAME "\": \"testevent\","
                        "\"" EVENT_VALUE "\": 1"
                        "}";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    ck_assert((unsigned long) s == strlen(buf));
    ck_assert(messagebuf != NULL);

    ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_EVENT);

    struct ws_event* e = (struct ws_event*) messagebuf;

    { // compare names
        struct ws_string* cmp = ws_string_new();
        ck_assert(cmp);

        ws_string_set_from_raw(cmp, "testevent");

        ck_assert(0 == ws_string_cmp(&e->name, cmp));

        ws_object_unref((struct ws_object*) cmp);
    }

    ck_assert(e->context.value.type == WS_VALUE_TYPE_INT);
    ck_assert(e->context.int_.i == 1);
}
END_TEST

START_TEST (test_json_deserializer_multiple_transactions) {
    char const* buf =   "{"
                        "\"" TYPE "\": \"" TYPE_EVENT "\","
                        "\"" EVENT_NAME "\": \"testevent1\","
                        "\"" EVENT_VALUE "\": 1"
                        "}"
                        "{"
                        "\"" TYPE "\": \"" TYPE_EVENT "\","
                        "\"" EVENT_NAME "\": \"testevent2\","
                        "\"" EVENT_VALUE "\": 2"
                        "}";

    ssize_t s = ws_deserialize(d, &messagebuf, buf, strlen(buf));

    {
        ck_assert(messagebuf != NULL);

        ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_EVENT);

        struct ws_event* e = (struct ws_event*) messagebuf;

        { // compare names
            struct ws_string* cmp = ws_string_new();
            ck_assert(cmp);

            ws_string_set_from_raw(cmp, "testevent1");

            ck_assert(0 == ws_string_cmp(&e->name, cmp));

            ws_object_unref((struct ws_object*) cmp);
        }

        ck_assert(e->context.value.type == WS_VALUE_TYPE_INT);
        ck_assert(e->context.int_.i == 1);

    }

    s = ws_deserialize(d, &messagebuf, buf + s, strlen(buf) - s);

    {
        ck_assert(messagebuf != NULL);

        ck_assert(messagebuf->obj.id == &WS_OBJECT_TYPE_ID_EVENT);

        struct ws_event* e = (struct ws_event*) messagebuf;

        { // compare names
            struct ws_string* cmp = ws_string_new();
            ck_assert(cmp);

            ws_string_set_from_raw(cmp, "testevent2");

            ck_assert(0 == ws_string_cmp(&e->name, cmp));

            ws_object_unref((struct ws_object*) cmp);
        }

        ck_assert(e->context.value.type == WS_VALUE_TYPE_INT);
        ck_assert(e->context.int_.i == 2);

    }

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
    tcase_add_test(tcx, test_json_deserializer_json_object_kv_null);
    tcase_add_test(tcx, test_json_deserializer_json_object_kv_bool);
    tcase_add_test(tcx, test_json_deserializer_json_object_kv_int);
    tcase_add_test(tcx, test_json_deserializer_json_object_kv_string);
    tcase_add_test(tcx, test_json_deserializer_message_id);
    tcase_add_test(tcx, test_json_deserializer_transaction_type);
    tcase_add_test(tcx, test_json_deserializer_transaction_valid_nocmds);
    tcase_add_test(tcx, test_json_deserializer_transaction_one_command);
    tcase_add_test(tcx, test_json_deserializer_transaction_commands);
    tcase_add_test(tcx, test_json_deserializer_flags);
    tcase_add_test(tcx, test_json_deserializer_events);
    tcase_add_test(tcx, test_json_deserializer_events_with_type);
    tcase_add_test(tcx, test_json_deserializer_events_with_everything);
    tcase_add_test(tcx, test_json_deserializer_multiple_transactions);

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

