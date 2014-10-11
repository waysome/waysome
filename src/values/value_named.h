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

#ifndef __WS_VALUES_VALUE_NAMED_H__
#define __WS_VALUES_VALUE_NAMED_H__

#include <pthread.h>

#include "values/value.h"

/**
 * ws_value_named_value type definition
 *
 * @extends ws_value
 */
struct ws_value_named_value {
    struct ws_value value; //!< @protected Base class.

    struct ws_string* name; //!< @protected Name of values
    struct ws_value*  v; //!< @protected Value of the value
};

/**
 * Initialize a ws_value_named_value object
 *
 * @memberof ws_value_named_value
 */
void
ws_value_named_value_init(
    struct ws_value_named_value* self //!< The object
);

/**
 * Set the name of an ws_value_named_value object
 *
 * @memberof ws_value_named_value
 *
 * @note Removes the old string and releases the ref on it, gets a ref on the
 * new name string object
 *
 * @return zero on success, else negative error number from errno.h
 */
int
ws_value_named_set_name(
    struct ws_value_named_value* self, //!< The object
    struct ws_string* str //!< new name
);

/**
 * Set the value of an ws_value_named_value object
 *
 * @memberof ws_value_named_value
 *
 * @return zero on success, else negative error number from errno.h
 */
int
ws_value_named_set_value(
    struct ws_value_named_value* self, //!< The object
    struct ws_value* val //!< new value
);

/**
 * Get the name of an ws_value_named_value object
 *
 * @memberof ws_value_named_value
 *
 * @note Gets a ref for you from the name before returning
 *
 * @return The name of the object or NULL on failure
 */
struct ws_string const*
ws_value_named_get_name(
    struct ws_value_named_value* self //!< The object
);

/**
 * Get the value of an ws_value_named_value object
 *
 * @memberof ws_value_named_value
 *
 * @return The value of the object or NULL on failure
 */
struct ws_value*
ws_value_named_get_value(
    struct ws_value_named_value* self //!< The object
);

/**
 * Compare two ws_value_named_value objects (by name)
 *
 * @memberof ws_value_named_value
 *
 * @return -1 if the object itself is "bigger"
 *          0 if the objects are equal (have the same name)
 *          1 if the other object is "bigger"
 */
signed int
ws_value_named_cmp(
    struct ws_value_named_value* self, //!< The object
    struct ws_value_named_value* other //!< Object to compare to
);

#endif // __WS_VALUES_VALUE_NAMED_H__

/**
 * @}
 */
