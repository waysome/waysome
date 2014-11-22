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

#ifndef __WS_VALUES_UNION_H__
#define __WS_VALUES_UNION_H__

#include "values/bool.h"
#include "values/int.h"
#include "values/nil.h"
#include "values/object_id.h"
#include "values/set.h"
#include "values/string.h"
#include "values/value.h"
#include "values/value_named.h"

/**
 * Union for all the value types in existence
 */
union ws_value_union {
    struct ws_value             value;          //!< plain value
    struct ws_value_nil         nil;            //!< nil value
    struct ws_value_bool        bool_;          //!< bool value
    struct ws_value_int         int_;           //!< int value
    struct ws_value_string      string;         //!< string value
    struct ws_value_object_id   object_id;      //!< object id value
    struct ws_value_set         set;            //!< set value
    struct ws_value_named_value named_value;    //!< named value
};

/**
 * Initializes the union by copying another value
 *
 * Only valid values are copied, e.g. "none" and "value" are not.
 *
 * @return 0 on success, a negative error code otherwise
 */
int
ws_value_union_init_from_val(
    union ws_value_union* dest, //!< destination of the copy action
    struct ws_value* src //!< source of the value to copy
)
__ws_nonnull__(1, 2)
;

/**
 * Reinit a value union
 *
 * This method deinitializes a value union and reinitializes it with the type
 * supplied.
 *
 * @memberof ws_value_union
 *
 * @return 0 on success, a negative error code otherwise
 */
int
ws_value_union_reinit(
    union ws_value_union* self, //!< union to reinitialize
    enum ws_value_type type //!< type to initialize dest with
)
__ws_nonnull__(1)
;


#endif //__WS_VALUES_UNION_H__

/**
 * @}
 */

