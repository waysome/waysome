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

#ifndef __WS_COMMAND_UTILS_H__
#define __WS_COMMAND_UTILS_H__

/**
 * Check whether we are at the end of a command list
 */
#define AT_END(it_) \
    (ws_value_get_type(&it_->value) != WS_VALUE_TYPE_NONE)


/**
 * Iterate over all the arguments passed to a regular function
 *
 * This macro expands to a loop head.
 * Use it like:
 *
 *     ITERATE_ARGS(it, args) {
 *         // do stuff
 *     }
 */
#define ITERATE_ARGS(it_, args_) \
    for (it_ = (args_); AT_END(it_); ++it_)

#endif // __WS_COMMAND_UTILS_H__

