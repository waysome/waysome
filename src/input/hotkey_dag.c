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
#define DAG_TAB_MAX_DEPTH (sizeof(uint16_t) * 8 / DAG_TAB_CHILD_NUM_EXP - 1)


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

/**
 * Destruct a DAG node
 */
static void
destruct_dag_node(
    struct ws_hotkey_dag_node* node //!< node to destruct (and free)
)
__ws_nonnull__(1)
;

/**
 * Destruct a tab node
 */
static void
destruct_tab_node(
    void* tab_node, //!< tab node to destruct
    uint8_t depth //!< depth of the node to destruct
);

/**
 * Remove an event from the tree given by it's root
 *
 * @note the removal is done by simple nullification.
 *
 * @return 0 on success, a negative error value on error
 */
static struct ws_hotkey_dag_tab
removal_point_in_tab_node(
    struct ws_hotkey_dag_tab tab, //!< table to remove the event from
    uint_fast16_t code //!< remove a code from a node tree
);

/**
 * Flatten TAB
 *
 * Flatten a TAB tree by selecting a new root
 */
static void
flatten_tab(
    struct ws_hotkey_dag_tab* tab //!< table to flatten
)
__ws_nonnull__(1)
;


/**
 * Count number of pointers set in a tab node
 *
 * @return number of pointers set
 */
static int
get_tab_node_numof_children(
    void** node //!< table node
)
__ws_nonnull__(1)
;


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
    destruct_tab_node(entry_node->table.nodes.tab, entry_node->table.depth);
    entry_node->table.nodes.tab = NULL;
    entry_node->table.start = 0;
    entry_node->table.depth = 0;

    // deinitialize events
    if (entry_node->event) {
        ws_object_unref(&entry_node->event->obj);
    }
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

    // check whether the code is in the range covered by the tree
    if ((code < cur.start) ||
            (((code - cur.start) >> step) >= DAG_TAB_CHILD_NUM)) {
        return NULL;
    }

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
    struct ws_hotkey_dag_node* last_keep = node;
    int rem_code = -1;

    // approach the node containing the event
    {
        size_t code_num = event->code_num;
        uint16_t* code = event->codes;
        while (code_num--) {
            if (!node) {
                return -EEXIST;
            }

            // check whether this node _may_ be removed
            if ((node->table.depth > 0) ||
                    (get_tab_node_numof_children(node->table.nodes.tab) > 1)) {
                // nope
                last_keep = node;
                rem_code = *code;
            }

            // get the next node, asserting it is set
            node = ws_hotkey_dag_next(node, *code);
            ++code;
        }
    }

    // now we have the node. Let's check for the event and unref it
    if (!(node && node->event)) {
        return -EEXIST;
    }
    ws_object_unref(&node->event->obj);
    node->event = NULL;

    // check whether to keep the node
    if (node->table.nodes.tab &&
            (get_tab_node_numof_children(node->table.nodes.tab) > 0)) {
        // yep, we're done
        return 0;
    }

    // check whether the entire tree may be purged
    if (rem_code < 0) {
        // yep
        ws_hotkey_dag_deinit(last_keep);
        return 0;
    }

    // get the point where we may start the removal and commence destruction!
    struct ws_hotkey_dag_tab tab;
    memset(&tab, 0, sizeof(tab));

    tab = removal_point_in_tab_node(last_keep->table, rem_code);
    if (tab.nodes.tab) {
        // we have a node to return
        destruct_tab_node((void*) tab.nodes.tab, tab.depth);
        flatten_tab(&last_keep->table);
    } else if (last_keep->table.depth == 0) {
        // the root node holds a ref to a DAG node which we have to remove
        struct ws_hotkey_dag_node** to_del;
        to_del = last_keep->table.nodes.dag +
                 (rem_code - last_keep->table.start);

        destruct_dag_node(*to_del);
        *to_del = NULL;
    }

    return 0;
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
        tab->depth = 0;
        tab->start = code & ~(DAG_TAB_CHILD_NUM - 1);
        return 0;
    }

    // step on which the node is based.
    int step = DAG_TAB_CHILD_NUM_EXP * tab->depth;

    // position within the node
    size_t pos = (code - tab->start) >> step;

    // extend the table "upwards", if necessary
    while ((code < tab->start) || (pos > DAG_TAB_CHILD_NUM)) {
        // we have to create a new node
        void** tab_node = create_tab_node();
        if (!tab_node) {
            return -ENOMEM;
        }

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

static void
destruct_dag_node(
    struct ws_hotkey_dag_node* node
) {
    // deinit and free
    ws_hotkey_dag_deinit(node);
    free(node);
}

static void
destruct_tab_node(
    void* tab_node,
    uint8_t depth
) {
    if (!tab_node || (depth > DAG_TAB_MAX_DEPTH)) {
        // nothing to do
        return;
    }

    --depth;

    void** cur_node = ((void**) tab_node) + DAG_TAB_CHILD_NUM;
    // iterate over all the nodes
    while (cur_node-- > (void**) tab_node) {
        // destruct children
        if (depth) {
            destruct_tab_node(*cur_node, depth);
        } else {
            destruct_dag_node((struct ws_hotkey_dag_node*) cur_node);
        }
    }

    // free this bit of memory
    free(tab_node);
}

static struct ws_hotkey_dag_tab
removal_point_in_tab_node(
    struct ws_hotkey_dag_tab tab,
    uint_fast16_t code
) {
    // prepare the return value
    struct ws_hotkey_dag_tab retval;
    memset(&retval, 0, sizeof(retval));
    retval.nodes.tab = NULL;
    retval.depth = ~0;

    void** rem_ptr = NULL;

    // step on which the node is based.
    int step = DAG_TAB_CHILD_NUM_EXP * tab.depth;

    // move towards the bottom,
    while (tab.depth) {
        // determine where to go next...
        size_t pos = (code - tab.start) >> step;
        void** next_tab = tab.nodes.tab + pos;
        if (!*next_tab) {
            return retval;
        }

        // regenerage all the variables
        tab.nodes.tab = *next_tab;
        tab.start = code & ~((1 << step) - 1);
        step -= DAG_TAB_CHILD_NUM_EXP;
        --tab.depth;

        // we consider clearing this node if we remove the _last_ child
        if (get_tab_node_numof_children(tab.nodes.tab) > 1) {
            // invalidate the retval
            retval.nodes.tab = NULL;
            retval.depth = ~0;
        } else if (!retval.nodes.tab) {
            retval = tab;
            rem_ptr = next_tab;
        }
    }

    // unlink the tab we'll return from it's super-tree
    if (rem_ptr) {
        *rem_ptr = NULL;
    }

    return retval;
}

static void
flatten_tab(
    struct ws_hotkey_dag_tab* tab
) {
    // we try to pull out as many roots as we can
    while (tab->depth) {
        // look for _the one_ child
        void** child = tab->nodes.tab + DAG_TAB_CHILD_NUM;
        while ( child-- > tab->nodes.tab) {
            if (*child) {
                break;
            }
        }

        void** ck = child;
        while (ck-- > tab->nodes.tab) {
            if (*ck) {
                // I said _one_ goddammit!
                return;
            }
        }

        // if we have only one child, we can easily pull out the old root
        size_t pos = (child - tab->nodes.tab) <<
                     (DAG_TAB_CHILD_NUM_EXP * tab->depth);
        free(tab->nodes.tab);
        tab->nodes.tab = child;

        // we have to adjust depth and start
        --tab->depth;
        tab->start = tab->start + pos;
    }
}

static int
get_tab_node_numof_children(
    void** node
) {
    if (!node) {
        return 0;
    }

    int non_null = 0;
    void** ck = node + DAG_TAB_CHILD_NUM;
    while (ck-- > node) {
        non_null += (*ck != NULL);
    }
    return non_null;
}

