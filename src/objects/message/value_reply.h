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

#ifndef __WS_OBJECTS_VALUE_REPLY_H__
#define __WS_OBJECTS_VALUE_REPLY_H__

#include "objects/message/reply.h"
#include "values/union.h"


// Forward declarations
struct ws_transaction;


/**
 * Reply message type
 *
 * This reply type is intended for transportation of return values back to the
 * source of the transaction.
 *
 * @extends ws_reply
 */
struct ws_value_reply {
    struct ws_reply reply; //!< @protected base class
    union ws_value_union value; //!< @protected value type
};

/**
 * Variable which holds the type information about the ws_value_reply type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_VALUE_REPLY;

/**
 * Create a value reply
 *
 * This function creates a value reply from a transaction, initializing the
 * embedded value with `value`.
 *
 * @return a new reply of `NULL`, if an error occurred
 */
struct ws_value_reply*
ws_value_reply_new(
    struct ws_transaction* src, //!< transaction for which this is the reply
    struct ws_value* value //!< value to return. `NULL` for a nil value
)
__ws_nonnull__(1)
;

/**
 * Get the embedded value
 *
 * @return the value embedded in the reply
 */
struct ws_value*
ws_value_reply_get_value(
    struct ws_value_reply* self
)
__ws_nonnull__(1)
;

#endif // __WS_OBJECTS_VALUE_REPLY_H__

