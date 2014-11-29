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

#ifndef __WS_UTIL_STRING_H__
#define __WS_UTIL_STRING_H__


#include <string.h>

/**
 * Check if two strings are equal
 *
 * @return true if the two strings are equal, else false
 */
#define ws_streq(str1, str2) \
    (0 == strcmp((str1), (str2)))

/**
 * Check if the first `n` bytes of `str1` and `str2` are equal
 *
 * @return true if the bytes are equal, else false
 */
#define ws_strneq(str1, str2, n) \
    (0 == strncmp((str1), (str2), (n)))


#endif // __WS_UTIL_STRING_H__

/**
 * @}
 */

