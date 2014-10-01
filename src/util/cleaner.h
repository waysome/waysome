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

#ifndef __WS_UTIL_CLEANER_H__
#define __WS_UTIL_CLEANER_H__

#include <stdbool.h>

/**
 * Initialize the cleaner
 */
void
ws_cleaner_init(void);

/**
 * Add a function to the cleaner
 *
 * @note Threadsafe!
 *
 * @return zero or negative error code from errno.h
 */
int
ws_cleaner_add(
    void (*cleaner_func)(void*), //!< Cleanup funtion
    void* etc //!< Argument for the cleaner. Is not allowed to be NULL
);

/**
 * Run the cleaner
 *
 * @warning It is _not_ safe to use _any_ memory which was cleaned up after this
 * method ran.
 *
 * @warning This function is not thread safe.
 *
 * @return true if all cleaners were executed, else false.
 */
bool
ws_cleaner_run(void);

#endif // __WS_UTIL_CLEANER_H__

/**
 * @}
 */
