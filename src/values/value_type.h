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

#ifndef __WS_VALUES_VALUE_TYPE_H__
#define __WS_VALUES_VALUE_TYPE_H__

/**
 * Type identifiers for value objects
 */
enum ws_value_type {
    WS_VALUE_TYPE_NONE,     //!< Indicator: This is not a value object anymore
    WS_VALUE_TYPE_VALUE,
    WS_VALUE_TYPE_NIL,
    WS_VALUE_TYPE_BOOL,
    WS_VALUE_TYPE_INT,
    WS_VALUE_TYPE_STRING,
    WS_VALUE_TYPE_OBJECT_ID,
    WS_VALUE_TYPE_SET,
    WS_VALUE_TYPE_NAMED,
};


#endif // __WS_VALUES_VALUE_TYPE_H__

/**
 * @}
 */
