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

#ifndef __WS_VALUES_INT_H__
#define __WS_VALUES_INT_H__

#include <pthread.h>
#include <stdint.h>

#include "values/value.h"

/**
 * ws_value_int type definition
 *
 * @extends ws_value
 *
 * The implementation for the int type uses an 32-bit wide integer type
 * internally.
 */
struct ws_value_int {
    struct ws_value value;

    uint32_t i;
};

/**
 * Initialize a ws_value_int object
 *
 * @memberof ws_value_int
 */
void
ws_value_int_init(
    struct ws_value_int* self //!< The object
);

/**
 * Set an ws_value_int object to a specific value
 *
 * @memberof ws_value_int
 *
 * @return zero on success, else negative error constant from errno.h
 */
int
ws_value_int_set(
    struct ws_value_int* self, //!< The object
    uint32_t i //!< The value to set to
);

/**
 * Get the value of the ws_value_int object
 *
 * @memberof ws_value_int
 *
 * @return the value of the object
 */
uint32_t
ws_value_int_get(
    struct ws_value_int* self //!< The object
);

/**
 * Compare two ws_value_int objects
 *
 * @memberof ws_value_int
 *
 * @return -1, 0 or 1 if the own object is bigger, both are equal or the other
 * is bigger, respecively
 */
signed int
ws_value_int_cmp(
    struct ws_value_int* self, //!< The object
    struct ws_value_int* other //!< The other object
);

#endif // __WS_VALUES_INT_H__

/**
 * @}
 */
