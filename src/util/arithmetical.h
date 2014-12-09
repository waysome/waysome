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
 *
 * @copydoc doc_utility
 */

#ifndef __WS_UTIL_ARITHMETICAL_H__
#define __WS_UTIL_ARITHMETICAL_H__

/**
 * Returns the sign of the number put in
 *
 * @returns 1 if x is positive, -1 if negative, and 0 if zero
 */
#define signum(x) (((x) > 0) - ((x) < 0))

/**
 * Returns the bigger of the two numbers
 *
 * @returns x if x>y else y
 */
#define MAX(x,y) ((x) > (y) ? (x) : (y))

/**
 * Returns the smaller of the two numbers
 *
 * @returns x if x<y else y
 */
#define MIN(x,y) ((x) < (y) ? (x) : (y))

/**
 * Returns the clamped value between the two arguments
 *
 * @returns y if x <= y <= z else if y > z, z else x
 */
#define CLAMP(x,y,z) MAX(x,MIN(y,z))

/**
 * Get the length of an array
 *
 * @returns The length of the array
 */
#define ARYLEN(x) (sizeof((x))/sizeof((x)[0]))

#endif // __WS_UTIL_ARITHMETICAL_H__

/**
 * @}
 */
