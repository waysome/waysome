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

#ifndef __WS_INPUT_HOTKEY_DAG_H__
#define __WS_INPUT_HOTKEY_DAG_H__

#include <stdint.h>

#include "util/attributes.h"

// forward declarations
struct ws_hotkey_dag_tab_node;
struct ws_hotkey_event;


/**
 * DAG node table
 *
 * This is a data type which gives the "table nodes" structure.
 * It tells you two things: the `depth` of the tree and the start of the range
 * covered by the table.
 * See @ref hotkeys for more information.
 */
struct ws_hotkey_dag_tab {
    union {
        struct ws_hotkey_dag_node** dag;
        void** tab;
    } nodes; //!< @protected nodes in this tab
    uint16_t start; //!< @protected stating point in the code space
    uint8_t depth; //!< @protected indicator on how far we are from dag nodes
};

/**
 * Hotkey DAG node type
 *
 * See @ref hotkeys for more information.
 *
 * @note Pointers of this type are used as handles to carry state
 */
struct ws_hotkey_dag_node {
    struct ws_hotkey_event* event; //!< @protected event stored in this node
    struct ws_hotkey_dag_tab table; //!< @protected table containing more nodes
};


/**
 * Initialize a new hotkey DAG
 *
 * This function initializes a DAG
 *
 * @return 0 if the initialization was successful, a negative error number
 *         otherwise
 */
int
ws_hotkey_dag_init(
    struct ws_hotkey_dag_node* entry_node
)
__ws_nonnull__(1)
;

/**
 * Destruct a DAG
 */
void
ws_hotkey_dag_deinit(
    struct ws_hotkey_dag_node* node
)
__ws_nonnull__(1)
;

/**
 * Insert an event in a DAG
 *
 * @return 0 if the operation was successful, a negative error number otherwise
 */
int
ws_hotkey_dag_insert(
    struct ws_hotkey_dag_node* entry_node,
    struct ws_hotkey_event* event
)
__ws_nonnull__(1, 2)
;

/**
 * Remove an event from a DAG
 *
 * @return 0 if the operation was successful, a negative error number otherwise
 */
int
ws_hotkey_dag_remove(
    struct ws_hotkey_dag_node* node,
    struct ws_hotkey_event* event
)
__ws_nonnull__(1, 2)
;

#endif // __WS_INPUT_HOTKEY_DAG_H__

