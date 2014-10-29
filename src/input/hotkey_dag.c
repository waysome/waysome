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

#include <errno.h>
#include <malloc.h>
#include <string.h>

#include "input/hotkey_dag.h"


#define DAG_TAB_CHILD_NUM_EXP (4)
#define DAG_TAB_CHILD_NUM (1 << DAG_TAB_CHILD_NUM_EXP)


/*
 *
 * Interface implementation
 *
 */

int
ws_hotkey_dag_init(
    struct ws_hotkey_dag_node* entry_node
) {
    // it's really as simple as...
    memset(entry_node, 0, sizeof(*entry_node));
    return 0;
}

void
ws_hotkey_dag_deinit(
    struct ws_hotkey_dag_node* entry_node
) {
    //!< @todo implement (now _this_ will get complicated...)
}

int
ws_hotkey_dag_insert(
    struct ws_hotkey_dag_node* node,
    struct ws_hotkey_event* event
) {
    //!< @todo iterate over all the permutations possible, inserting
    return -1;
}

int
ws_hotkey_dag_remove(
    struct ws_hotkey_dag_node* node,
    struct ws_hotkey_event* event
) {
    //!< @todo remove an event, then all nodes linking to it, ...
    return -1;
}


