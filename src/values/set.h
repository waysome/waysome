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
 * @addtogroup values "Value types"
 *
 * @{
 */

#ifndef __WS_VALUES_SET_H__
#define __WS_VALUES_SET_H__

#include "objects/object.h"
#include "objects/set.h"
#include "values/value.h"

/**
 * ws_value_set type definiton
 *
 * The implementation of the set type used for the api and the transactions.
 */
struct ws_value_set {
    struct ws_value value; //!< @protected base class
    struct ws_set* set; //!< @protected ws_set object
};

/**
 * Predicate function type
 */
typedef int (*ws_value_set_predf)(void const*, void*);

/**
 * Processor function type
 */
typedef int (*ws_value_set_procf)(void*, void const*);

/**
 * Initialize a value_set object
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error code from errno.h
 */
int
ws_value_set_init(
    struct ws_value_set* self //!< The value_set object
)
__ws_nonnull__(1)
;

/**
 * Get a new, initialized value_set object
 *
 * @memberof ws_value_set
 *
 * @return new value_set object or NULL on failure
 */
struct ws_value_set*
ws_value_set_new(void);

/**
 * Insert an object into the set
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_insert(
    struct ws_value_set* self, //!< The value_set object
    struct ws_object* obj //!< The object to insert
)
__ws_nonnull__(1, 2)
;


/**
 * Remove an object from the value_set by a unique identification
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_remove(
    struct ws_value_set* self, //!< The value_set object
    struct ws_object const* cmp //!< The object to compare to
)
__ws_nonnull__(1, 2)
;

/**
 * Get an object from a value_set
 *
 * @memberof ws_value_set
 *
 * @return the ws_object object or NULL on failure
 */
struct ws_object*
ws_value_set_get_object(
    struct ws_value_set const* self, //!< The value_set object
    struct ws_object const* cmp //!< The object to compare to
)
__ws_nonnull__(1, 2)
;

/**
 * Create an union from two value_set objects
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_union(
    struct ws_value_set* dest, //!< The destination object
    struct ws_value_set const* src_a, //!< The first source value_set
    struct ws_value_set const* src_b //!< The second source value_set
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Create an intersection from two value_sets
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_intersection(
    struct ws_value_set* dest, //!< The destination object
    struct ws_value_set const* src_a, //!< The first source value_set
    struct ws_value_set const* src_b //!< The second source value_set
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Create the symmetric difference between two value_sets
 *
 * @memberof ws_value_set
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_xor(
    struct ws_value_set* dest, //!< The destination object
    struct ws_value_set const* src_a, //!< The first source value_set
    struct ws_value_set const* src_b //!< The second source value_set
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Check if one value_set is a subset of another
 *
 * @memberof ws_value_set
 *
 * @return true if `other` is subset of `self`, else false
 */
bool
ws_value_set_is_subset(
    struct ws_value_set const* self, //!< The value_set object
    struct ws_value_set const* other //!< The value_set object to compare to
)
__ws_nonnull__(1, 2)
;

/**
 * Check if two value_sets are equal
 *
 * @memberof ws_value_set
 *
 * @return true if `self` and `other` are equal, else false
 */
bool
ws_value_set_equal(
    struct ws_value_set const* self, //!< The value_set object
    struct ws_value_set const* other //!< The value_set object to compare to
)
__ws_nonnull__(1, 2)
;

/**
 * Get the cardinality of a value_set
 *
 * @memberof ws_value_set
 *
 * @return the cardinality of `self`
 */
size_t
ws_value_set_cardinality(
    struct ws_value_set const* self //!< The value_set object
)
__ws_nonnull__(1)
;

/**
 * Execute a processor function for each element of a value_set
 *
 * @memberof ws_value_set
 *
 * @note `ws_value_set_select` can be used to execute an operation on a subset
 * of a value_set object, selecting elements for another value_set or doing
 * other crazy things for elements of a value_set.
 *
 * @return zero on success, else negative error value from errno.h
 */
int
ws_value_set_select(
    struct ws_value_set const* self, //!< The value_set object
    ws_value_set_predf pred, //!< Predicate function
    void* pred_etc, //!< Additional parameter for the predicate function
    ws_value_set_procf proc, //!< Processor function
    void* proc_etc //!<Additional parameter for the processor function
)
__ws_nonnull__(1)
;

#endif // __WS_VALUES_SET_H__

/**
 * @}
 */
