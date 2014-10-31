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
#include "input/hotkey_event.h"


#define DAG_TAB_CHILD_NUM_EXP (4)
#define DAG_TAB_CHILD_NUM (1 << DAG_TAB_CHILD_NUM_EXP)


/*
 *
 * Forward declarations
 *
 */

/**
 * Get the next DAG node, creating it if it does not exist
 */
static struct ws_hotkey_dag_node*
hotkey_dag_get(
    struct ws_hotkey_dag_node* node, //!< node from which to get the next one
    uint16_t code //!< code of the new next node
)
__ws_nonnull__(1)
;

/**
 * Make sure a root may have a child suitable for a code
 *
 * @return 0 if the root is suitable for storing the code is initialized,
 *         a negative error number otherwise
 */
static int
add_roots_for(
    struct ws_hotkey_dag_tab* tab, //!< tab which must be extended
    uint16_t code //!< code which must be storable under the root
)
__ws_nonnull__(1)
;

/**
 * Create new tab node
 */
static void**
create_tab_node(void);


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

struct ws_hotkey_dag_node*
ws_hotkey_dag_next(
    struct ws_hotkey_dag_node* node,
    uint16_t code
) {
    // you must not pass NULL, but we want to be save in _this_ case
    if (!node) {
        return NULL;
    }

    struct ws_hotkey_dag_tab cur = node->table;

    uint16_t step = DAG_TAB_CHILD_NUM_EXP * cur.depth;

    // move towards the bottom
    while (cur.depth-- && cur.nodes.tab) {
        // determine where to go next
        size_t pos = (code - cur.start) >> step;
        cur.nodes.tab = cur.nodes.tab[pos];

        // update start and step
        cur.start = cur.start | (pos << step);
        step -= DAG_TAB_CHILD_NUM_EXP;
    }

    // catch possible errors
    if (!cur.nodes.dag) {
        return NULL;
    }

    return cur.nodes.dag[code - cur.start];
}

int
ws_hotkey_dag_insert(
    struct ws_hotkey_dag_node* node,
    struct ws_hotkey_event* event
) {
    uint16_t* code = event->codes;
    uint16_t num = event->code_num;

    // traverse the tree, creating nodes where neccessary
    while (num--) {
        node = hotkey_dag_get(node, *code);
        ++code;
    }

    if (node->event) {
        // the node already exists
        return -EEXIST;
    }

    // finally, insert the event
    node->event = getref(event);
    return 0;
}

int
ws_hotkey_dag_remove(
    struct ws_hotkey_dag_node* node,
    struct ws_hotkey_event* event
) {
    //!< @todo remove an event, then all nodes linking to it, ...
    return -1;
}


/*
 *
 * Internal implementation
 *
 */

static struct ws_hotkey_dag_node*
hotkey_dag_get(
    struct ws_hotkey_dag_node* node,
    uint16_t code
) {
    // make sure the root is sane for the code we want to insert
    if (add_roots_for(&node->table, code) < 0) {
        return NULL;
    }

    struct ws_hotkey_dag_tab cur = node->table;

    // step on which the node is based.
    int step = DAG_TAB_CHILD_NUM_EXP * cur.depth;

    // move towards the bottom
    while (cur.depth) {
        // determine where to go next...
        void** tab = cur.nodes.tab + ((code - cur.start) >> step);

        // initializing the node if neccessary
        if (!*tab) {
            *tab = create_tab_node();
            if (!*tab) {
                return NULL;
            }
        }

        // regenerage all the variables
        cur.nodes.tab = *tab;
        cur.start = code & ~((1 << step) - 1);
        step -= DAG_TAB_CHILD_NUM_EXP;
        --cur.depth;
    }

    struct ws_hotkey_dag_node** retp = cur.nodes.dag + (code - cur.start);
    if (!*retp) {
        *retp = malloc(sizeof(*retp));
        if (ws_hotkey_dag_init(*retp) < 0) {
            free(*retp);
            *retp = NULL;
        }
    }

    return *retp;
}

static int
add_roots_for(
    struct ws_hotkey_dag_tab* tab,
    uint16_t code
) {

    // the table _might_ be completely empty
    if (!tab->nodes.tab) {
        tab->nodes.tab = create_tab_node();
        if (!tab->nodes.tab) {
            return -ENOMEM;
        }
        if (tab->depth == 0) {
            tab->start = code & ~(DAG_TAB_CHILD_NUM - 1);
        }
    }

    // step on which the node is based.
    int step = DAG_TAB_CHILD_NUM_EXP * tab->depth;

    // position within the node
    size_t pos = (code - tab->start) >> step;

    // extend the table "upwards", if necessary
    while ((code < tab->start) || (pos > DAG_TAB_CHILD_NUM)) {
        // we have to create a new node
        void** tab_node = create_tab_node();

        size_t old_root_pos = (tab->start >> step);

        // put in the new root
        tab_node[old_root_pos  & (DAG_TAB_CHILD_NUM - 1)] = tab->nodes.tab;
        tab->nodes.tab = tab_node;
        ++tab->depth;

        // regen step, pos and start
        step += DAG_TAB_CHILD_NUM_EXP;
        tab->start = old_root_pos << step;
        pos >>= DAG_TAB_CHILD_NUM_EXP;
    }

    return 0;
}

static void**
create_tab_node(void) {
    return calloc(DAG_TAB_CHILD_NUM, sizeof(void*));
}

