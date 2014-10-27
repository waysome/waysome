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

#ifndef __WS_VALUES_BOOL_H__
#define __WS_VALUES_BOOL_H__

#include <stdbool.h>

#include "values/value.h"

/**
 * ws_value_bool type definition
 *
 * @extends ws_value
 */
struct ws_value_bool {
    struct ws_value value; //!< @protected Base class.

    bool b; //!< @private Actual value
};

/**
 * Initialize a ws_value_bool object
 *
 * @memberof ws_value_bool
 */
void
ws_value_bool_init(
    struct ws_value_bool* self //!< The object
);

/**
 * Get a plain `bool` from the ws_value_bool object
 *
 * @memberof ws_value_bool
 *
 * @return The `bool` representation of the `ws_value_bool` object
 */
bool
ws_value_bool_get(
    struct ws_value_bool* self //!< The object
);

/**
 * Set the ws_value_bool to a specific value
 *
 * @memberof ws_value_bool
 *
 * @return zero if operation succeeded, else negative error const from errno.h
 */
int
ws_value_bool_set(
    struct ws_value_bool* self, //!< The object
    bool b //!< The value to set to
);

#endif // __WS_VALUES_BOOL_H__

/**
 * @}
 */
