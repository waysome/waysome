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

#include <linux/input.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "action/manager.h"
#include "input/hotkey_dag.h"
#include "input/hotkey_event.h"
#include "input/hotkeys.h"
#include "objects/message/event.h"
#include "util/cleaner.h"

/**
 * Hotkey subsystem context
 */
struct {
    struct ws_hotkey_dag_node root; //!< root of the DAG holding the events
    struct ws_hotkey_dag_node* state; //!< node of the DAG we're on
    struct wl_array events; //!< button events consumed while traversing
    int buttons_pressed; //!< button presses - button releases
} ws_hotkeys_ctx;

/*
 *
 * Forward declarations
 *
 */

/**
 * Cleanup the hotkeys subsystem
 */
static void
hotkeys_deinit(
    void* dummy
);

/**
 * Reset the event list
 *
 * @return the old event list
 */
static struct wl_array
eventlist_reset(void);

/**
 * Return an empty event list
 */
static struct wl_array
empty_eventlist(void);

/*
 *
 * Interface implementation
 *
 */

int
ws_hotkeys_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }
    int res;

    res = ws_hotkey_dag_init(&ws_hotkeys_ctx.root);
    if (!res) {
        goto cleanup;
    }
    ws_hotkeys_ctx.state = &ws_hotkeys_ctx.root;

    ws_cleaner_add(hotkeys_deinit, NULL);
    wl_array_init(&ws_hotkeys_ctx.events);
    ws_hotkeys_ctx.buttons_pressed = 0;

    is_init = true;
    return 0;

cleanup:
    return res;
}

struct wl_array
ws_hotkeys_eval(
    struct input_event* ev
) {
    // get the key code
    uint16_t code = ev->code;

    // insert the event into the track list
    {
        struct input_event* buf;
        buf = wl_array_add(&ws_hotkeys_ctx.events, sizeof(*ev));
        if (!buf) {
            return eventlist_reset();
        }
        *buf = *ev;
    }

    // check whether the key was pressed or released
    if (ev->value) {
        ++ws_hotkeys_ctx.buttons_pressed;

        // jump to the next DAG node
        ws_hotkeys_ctx.state = ws_hotkey_dag_next(ws_hotkeys_ctx.state, code);
        if (!ws_hotkeys_ctx.state) {
            // this key is not part of a keycombo
            return eventlist_reset();
        }

        return empty_eventlist();
    }

    // check whether we just removed the last key
    --ws_hotkeys_ctx.buttons_pressed;
    if (ws_hotkeys_ctx.buttons_pressed > 0) {
        // nope. Means the input still might be a key combo
        return empty_eventlist();
    }

    // we have something which may be a key combo. Let's find out what to do
    if (!ws_hotkeys_ctx.state->event) {
        // nothing!
        return eventlist_reset();
    }

    // construct the event to emit
    struct ws_event* event;
    event = ws_event_new(&ws_hotkeys_ctx.state->event->name, NULL);
    if (!event) {
        return eventlist_reset();
    }

    // emit the event
    struct ws_reply* reply;
    reply = ws_action_manager_process((struct ws_message*) event);
    if (reply) {
        ws_object_unref((struct ws_object*) reply);
    }

    // reset the eventlist
    wl_array_release(&ws_hotkeys_ctx.events);
    wl_array_init(&ws_hotkeys_ctx.events);
    ws_hotkeys_ctx.buttons_pressed = 0;

    return ws_hotkeys_ctx.events;
}

/*
 *
 * Internal implementation
 *
 */

static void
hotkeys_deinit(
    void* dummy
) {
    wl_array_release(&ws_hotkeys_ctx.events);
    ws_hotkey_dag_deinit(&ws_hotkeys_ctx.root);
}

static struct wl_array
eventlist_reset(void) {
    struct wl_array buf = ws_hotkeys_ctx.events;
    ws_hotkeys_ctx.buttons_pressed = 0;
    wl_array_init(&ws_hotkeys_ctx.events);
    return buf;
}

static struct wl_array
empty_eventlist(void) {
    struct wl_array buf;
    wl_array_init(&buf);
    return buf;
}

