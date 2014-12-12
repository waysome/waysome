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
 * @addtogroup utils "(internal) utilities"
 *
 * @{
 */

/**
 * @addtogroup utils_error "(internal) utils: error utilities"
 *
 * @{
 */

#ifndef __WS_UTIL_ERROR_H__
#define __WS_UTIL_ERROR_H__

/**
 * Translate an errno number to a char*.
 *
 * @note Both positive and negative errno values can be passed
 *
 * @note The returned value can be free()'d afterwards
 *
 * @note This function will change later on, to be able to translate custom
 * errno number to sane error strings. Its interface won't change, though.
 *
 * @return Error description for the errno passed, NULL on failure (no memory)
 */
char*
ws_errno_tostr(
    int errno
);

/**
 * Create a `struct ws_value_string` instance from the errno number using
 * `ws_errno_tostr`
 *
 * @return `struct ws_value_string` for the errno number or NULL on failure
 */
struct ws_value_string*
ws_errno_to_value_string(
    int errno
);

#endif // __WS_UTIL_ERROR_H__

/**
 * @}
 */

/**
 * @}
 */
