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
 * @addtogroup objects "Classes"
 *
 * @{
 */

/**
 * @addtogroup objects_array "Class: Array type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_ARRAY_H__
#define __WS_OBJECTS_ARRAY_H__

#include <stdbool.h>
#include <pthread.h>

#include "objects/object.h"

/**
 * ws_array type definition
 *
 * @extends ws_object
 *
 * The ws_array type implementation operates on `void*` internally.
 */
struct ws_array {
    struct ws_object obj; //!< @protected Base class.

    size_t len; //!< @protected Length of the array
    size_t nused; //!< @protected Number of used elements in the array
    bool sorted; //! @protected Flag whether the array is sorted

    struct ws_object** ary; //!< @protected Actual array
};

/**
 * Variable which holds the type information about the ws_array type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_ARRAY;

/**
 * Initialize an array object
 *
 * @memberof ws_array
 *
 * @return zero on success, else negative error number from errno.h
 */
int
ws_array_init(
    struct ws_array* self //!< Array object
);

/**
 * Allocate a new, initialized array object
 *
 * @memberof ws_array
 *
 * @note One ref is already taken
 *
 * @return New array object, initialized. NULL on failure
 */
struct ws_array*
ws_array_new(void);

/**
 * Get the length of an array object
 *
 * @memberof ws_array
 *
 * @return Size of array object, zero on NULL passed
 */
size_t
ws_array_get_len(
    struct ws_array* const self //!< Array object
);

/**
 * Sort an array object
 *
 * @memberof ws_array
 *
 * @note Uses object sort mechanism
 */
void
ws_array_sort(
    struct ws_array* self //!< Array object
);

/**
 * Check if an array has an object
 *
 * @memberof ws_array
 *
 * @return true if the array has an reference to the object
 */
bool
ws_array_has(
    struct ws_array* const self, //!< Array object
    struct ws_object* const obj //!< Object to search for
);

/**
 * Find an object in the array by predicate
 *
 * @memberof ws_array
 *
 * @note gets a ref for you if it returns an object
 *
 * @return the object or NULL on failure or not found
 */
struct ws_object*
ws_array_find(
    struct ws_array* const self, //!< Array object
    struct ws_object const* cmp //!< Object to compare to
);

/**
 * Get from array at specific position
 *
 * @memberof ws_array
 *
 * @note gets a ref for you if it returns an object
 *
 * @warning returns NULL if index is out of bounds
 *
 * @return the object at position `i`
 */
struct ws_object*
ws_array_get_at(
    struct ws_array* const self, //!< Array object
    unsigned int i //!< Index
);

/**
 * Set object into array at specific position
 *
 * @memberof ws_array
 *
 * @warning returns NULL if index is out of bounds
 *
 * @note Gets a ref on the object before storing it, but not when returning it
 *
 * @return Object which was inserted or object which was overridden by inserting
 * operation, if there was one.
 */
struct ws_object*
ws_array_set_at(
    struct ws_array* self, //!< Array object
    struct ws_object* obj, //!< object to insert
    unsigned int i //!< Index for insertion
);

/**
 * Iterate through an array
 *
 * @memberof ws_array
 *
 * @warning stops iteration whenever `iter` returns false
 *
 * @note Gets a ref on the object before passing it to the iteration function
 * and releases the ref afterwards
 *
 * @return true if the iteration exited normally (over elements was iterated),
 * else false
 */
bool
ws_array_foreach(
    struct ws_array* const self, //!< Array object
    bool (*iter)(void* etc, struct ws_object* entry), //!< Iteration callback
    void* etc //!< optional argument for iteration callback
);

/**
 * Append a value to the array
 *
 * @memberof ws_array
 *
 * @note Tries to resize the array if there is not enough space
 *
 * @note Gets a ref on the objects before storing it
 *
 * @warning Does not set empty entries, does _always_ append.
 *
 * @return 0 on success, else error numbers from errno.h
 *          EINVAL - on passing NULL
 *          ENOMEM - on realloc() failure. Old array is saved in this case.
 */
int
ws_array_append(
    struct ws_array* const self, //!< Array object
    struct ws_object* //!< object to append
);

#endif // __WS_OBJECTS_ARRAY_H__

/**
 * @}
 */

/**
 * @}
 */
