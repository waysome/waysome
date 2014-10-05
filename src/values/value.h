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

/**
 * @addtogroup values_value "Value types: Value type"
 *
 * @{
 */

#ifndef __WS_VALUES_VALUE_H__
#define __WS_VALUES_VALUE_H__

#include "values/value_type.h"
#include "util/attributes.h"

struct ws_value;

typedef void (*ws_value_type_deinit_callback)(struct ws_value*);

struct ws_value {
    enum ws_value_type type;
    ws_value_type_deinit_callback deinit_callback;
};

/**
 * Initialize a value object
 */
void
ws_value_init(
    struct ws_value* self //!< The value object to initialize
)
__ws_nonnull__(1)
;

/**
 * Deinit a value object
 */
void
ws_value_deinit(
    struct ws_value* self //!< The value object to deinitialize
)
__ws_nonnull__(1)
;

/**
 * Get the type of a ws_value object
 */
enum ws_value_type
ws_value_get_type(
    struct ws_value* self //!< The object to get the type of
)
__ws_nonnull__(1)
;

#endif // __WS_VALUES_VALUE_H__

/**
 * @}
 */

/**
 * @}
 */
