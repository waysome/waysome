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

#ifndef __WS_OBJECTS_ARRAY_H__
#define __WS_OBJECTS_ARRAY_H__

#include <stdbool.h>
#include <pthread.h>

#include "objects/object.h"

/**
 * ws_array type definition
 *
 * The ws_array type implementation operates on `void*` internally.
 */
struct ws_array {
    struct ws_object obj; //!< Supertype

    size_t len; //!< Length of the array
    size_t nused; //!< Number of used elements in the array
    bool sorted; //!< Flag whether the array is sorted

    void** ary; //!< Actual array
};

/**
 * Variable which holds the type information about the ws_array type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_ARRAY;

/**
 * Initialize an array object
 *
 * @return true if initialisation worked, else false
 */
bool
ws_array_init(
    struct ws_array* self //!< Array object
);

/**
 * Allocate a new, initialized array object
 *
 * @return New array object, initialized. NULL on failure
 */
struct ws_array*
ws_array_new(void);

/**
 * Get the length of an array object
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
 * @note Sorts ascending pointers if `cmp` is NULL
 */
void
ws_array_sort(
    struct ws_array* self, //!< Array object
    signed int (*cmp)(const void* , const void*) //!< compare function
);

/**
 * Check if an array has an object
 *
 * @return true if the array has an reference to the object
 */
bool
ws_array_has(
    struct ws_array* const self, //!< Array object
    void* const obj //!< Object to search for
);

/**
 * Find an object in the array by predicate
 *
 * @note Iterates through the array. If the `cmp` function returns non-false,
 * the function returns immediately.
 *
 * @return the object or NULL on failure or not found
 */
void*
ws_array_find(
    struct ws_array* const self, //!< Array object
    bool (*cmp)(void* const) //!< Predicate for finding the object
);

/**
 * Get from array at specific position
 *
 * @warning returns NULL if index is out of bounds
 *
 * @return the object at position `i`
 */
void*
ws_array_get_at(
    struct ws_array* const self, //!< Array object
    unsigned int i //!< Index
);

/**
 * Set object into array at specific position
 *
 * @warning returns NULL if index is out of bounds
 *
 * @return Object which was inserted or object which was overridden by inserting
 * operation, if there was one.
 */
void*
ws_array_set_at(
    struct ws_array* self, //!< Array object
    void* obj, //!< object to insert
    unsigned int i //!< Index for insertion
);

/**
 * Iterate through an array
 *
 * @warning stops iteration whenever `iter` returns false
 *
 * @return true if the iteration exited normally (over elements was iterated),
 * else false
 */
bool
ws_array_foreach(
    struct ws_array* const self, //!< Array object
    bool (*iter)(void* etc, void* entry), //!< Iteration callback
    void* etc //!< optional argument for iteration callback
);

/**
 * Append a value to the array
 *
 * @note Tries to resize the array if there is not enough space
 *
 * @warning Does not set empty entries, does _always_ append.
 *
 * @return 0 on success, else error numbers from errno.h
 */
int
ws_array_append(
    struct ws_array* const self, //!< Array object
    void* //!< object to append
);

#endif // __WS_OBJECTS_ARRAY_H__

