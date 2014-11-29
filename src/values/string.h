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

#ifndef __WS_VALUES_STRING_H__
#define __WS_VALUES_STRING_H__

#include "objects/string.h"

#include "values/value.h"

/**
 * ws_value_string type definition
 */
struct ws_value_string {
    struct ws_value val; //!< @protected Base class.
    struct ws_string* str; //!< @protected ws_string object
};

/**
 * Initialize a ws_value_string object
 *
 * @memberof ws_value_string
 */
void
ws_value_string_init(
    struct ws_value_string* self
)
__ws_nonnull__(1)
;

/**
 * Allocate a new, initialized ws_value_string
 *
 * @return a new, allocated and initialized ws_value_string objects,
 * NULL on failure
 */
struct ws_value_string*
ws_value_string_new(void);


/**
 * get the ws_value_string's ws_string object
 *
 * @memberof ws_value_string
 *
 * @return the ws_string object contained in the ws_value_string object,
 * NULL on failure
 */
struct ws_string*
ws_value_string_get(
    struct ws_value_string* self
);

/**
 * set the ws_string object contained in the ws_value_string
 *
 * @memberof ws_value_string
 */
void
ws_value_string_set_str(
    struct ws_value_string* self,
    struct ws_string* str
);

#endif // __WS_VALUES_STRING_H__

/**
 * @}
 */
