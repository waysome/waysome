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
 * @addtogroup tests_input_dag "Testing: Input DAG"
 *
 * @{
 */

#include <stdint.h>
#include <stdlib.h>

#include <check.h>

#include "tests.h"
#include "input/hotkey_dag.h"
#include "input/hotkey_event.h"
#include "objects/string.h"
#include "util/arithmetical.h"

static struct ws_string*
mkstr(
    char* lit
) {
    struct ws_string* s = ws_string_new();
    ck_assert(s);
    ws_string_set_from_raw(s, lit);
    return s;
}

START_TEST (test_input_dag_init) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    ck_assert(node);
    ck_assert(ws_hotkey_dag_init(node) == 0);
    free(node);
}
END_TEST

START_TEST (test_input_dag_get_node_from_empty_node) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    ck_assert(node);
    ck_assert(NULL == ws_hotkey_dag_next(node, 0));
    free(node);
}
END_TEST

START_TEST (test_input_dag_remove_from_empty_node) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    ck_assert(0 == ws_hotkey_dag_init(node));

    struct ws_string* s = ws_string_new();
    ck_assert(s);
    uint16_t codes[] = {4, 3};
    struct ws_hotkey_event* ev = ws_hotkey_event_new(s, codes, 2);

    ck_assert(node);
    ck_assert(0 != ws_hotkey_dag_remove(node, ev));
    free(node);
}
END_TEST

START_TEST (test_input_dag_insert_hotkey_events_into_dag) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    ck_assert(node);
    ck_assert(0 == ws_hotkey_dag_init(node));

    struct ws_string* tmpname = mkstr("hello");

    uint16_t codes1[]   = { 1, 2, 15 }; // This is a key-combo
    uint16_t ncodes1    = 3;
    struct ws_hotkey_event* ev1 = ws_hotkey_event_new(tmpname, codes1, ncodes1);
    ck_assert(ev1);

    uint16_t codes2[]   = { 42, 22, 5 }; // This is a key-combo
    uint16_t ncodes2    = 3;
    struct ws_hotkey_event* ev2 = ws_hotkey_event_new(tmpname, codes2, ncodes2);
    ck_assert(ev2);

    uint16_t codes3[]   = { 42, 22, 6 }; // This is a key-combo
    uint16_t ncodes3    = 3;
    struct ws_hotkey_event* ev3 = ws_hotkey_event_new(tmpname, codes3, ncodes3);
    ck_assert(ev3);

    ck_assert(0 == ws_hotkey_dag_insert(node, ev1));
    ck_assert(0 == ws_hotkey_dag_insert(node, ev2));
    ck_assert(0 == ws_hotkey_dag_insert(node, ev3));

    // try to run through the tree with the following key codes
    struct ws_hotkey_dag_node* nxt = NULL;

    uint16_t trav_codes_1_succ[] = { 1,    2,  15 };
    nxt = ws_hotkey_dag_next(node,  trav_codes_1_succ[0]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_1_succ[1]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_1_succ[2]);
    ck_assert(nxt != NULL);

    uint16_t trav_codes_2_succ[] = { 42,   22, 5 };
    nxt = ws_hotkey_dag_next(node,  trav_codes_2_succ[0]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_2_succ[1]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_2_succ[2]);
    ck_assert(nxt != NULL);

    uint16_t trav_codes_3_succ[] = { 42,   22, 6 };
    nxt = ws_hotkey_dag_next(node,  trav_codes_3_succ[0]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_3_succ[1]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_3_succ[2]);
    ck_assert(nxt != NULL);

    uint16_t trav_codes_4_fail[] = { 42,   22, 15 };
    nxt = ws_hotkey_dag_next(node,  trav_codes_4_fail[0]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_4_fail[1]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_4_fail[2]);
    ck_assert(nxt == NULL); // There is no keycode 42-22-15 combo

    uint16_t trav_codes_5_fail[] = { 1,    3,  15 };
    nxt = ws_hotkey_dag_next(node,  trav_codes_5_fail[0]);
    ck_assert(nxt != NULL);
    nxt = ws_hotkey_dag_next(nxt,   trav_codes_5_fail[1]);
    ck_assert(nxt == NULL); // There is no keycode 1-3-15 combo

    ws_object_unref((struct ws_object*) tmpname);
    ws_object_unref((struct ws_object*) ev1);
    ws_object_unref((struct ws_object*) ev2);
}
END_TEST

START_TEST (test_input_dag_insert_multiple) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    struct ws_hotkey_event* ev;
    struct ws_string* name = ws_string_new();
    struct ws_string* app = ws_string_new();

    ws_string_set_from_raw(name, "testevent");
    ws_hotkey_dag_init(node);
    uint16_t trav_codes[] = {1, 3, 24};
    ev = ws_hotkey_event_new(name, trav_codes, 3);

    //trying to insert event that is already in DAG
    ck_assert(ws_hotkey_dag_insert(node, ev) == 0);
    ck_assert(ws_hotkey_dag_insert(node, ev) != 0);

    //Remove an event, then try to insert it again
    ck_assert(ws_hotkey_dag_remove(node, ev) == 0);
    ck_assert(ws_hotkey_dag_insert(node, ev) == 0);

    free(node);
    ws_object_unref((struct ws_object*) ev);
    ws_object_unref((struct ws_object*) name);
}
END_TEST

START_TEST (test_input_dag_insert) {
    struct ws_hotkey_dag_node* node = calloc(1, sizeof(*node));
    struct ws_hotkey_event* ev;
    size_t ev_ref_count;

    ws_hotkey_dag_init(node);
    
    struct ws_string* name = ws_string_new();
    ws_string_set_from_raw(name, "testevent");

    uint16_t codes[] = {2 ,3};
    ev = ws_hotkey_event_new(name, codes, 2);
    ev_ref_count = ev->obj.ref_counting.refcnt;

    ck_assert(ev != NULL);
    ck_assert(ws_hotkey_dag_insert(node, ev) == 0);
    
    ck_assert(ev_ref_count + 1 == ev->obj.ref_counting.refcnt);
    ws_object_unref((struct ws_object*) ev);

    free(node);
    ws_object_unref((struct ws_object*) name);
}
END_TEST

static Suite*
input_dag_suite(void)
{
    Suite* s    = suite_create("Input DAG");
    TCase* tc   = tcase_create("main case");

    suite_add_tcase(s, tc);
    // tcase_add_checked_fixture(tc, setup, cleanup); // Not used yet

    tcase_add_test(tc, test_input_dag_init);
    tcase_add_test(tc, test_input_dag_insert);
    tcase_add_test(tc, test_input_dag_insert_multiple);
    tcase_add_test(tc, test_input_dag_get_node_from_empty_node);
    tcase_add_test(tc, test_input_dag_remove_from_empty_node);
    tcase_add_test(tc, test_input_dag_insert_hotkey_events_into_dag);

    return s;
}

WS_TESTS_CHECK_MAIN(input_dag_suite);

/**
 * @}
 */

/**
 * @}
 */
