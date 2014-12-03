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
    (ws_value_get_type(&it_->value) == WS_VALUE_TYPE_NONE)


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
    for (it_ = (args_); !AT_END(it_); ++it_)


/*
 * Ugly block of macros for pure conversion, because _someone_ insisted on using
 * uppercase names for his enum values...
 */
#define WS_VALUE_TYPE_none      WS_VALUE_TYPE_NONE
#define WS_VALUE_TYPE_value     WS_VALUE_TYPE_VALUE
#define WS_VALUE_TYPE_nil       WS_VALUE_TYPE_NIL
#define WS_VALUE_TYPE_bool      WS_VALUE_TYPE_BOOL
#define WS_VALUE_TYPE_int       WS_VALUE_TYPE_INT
#define WS_VALUE_TYPE_string    WS_VALUE_TYPE_STRING
#define WS_VALUE_TYPE_object_id WS_VALUE_TYPE_OBJECT_ID
#define WS_VALUE_TYPE_set       WS_VALUE_TYPE_SET


/**
 * Iterate over all the arguments passed to a regular function, type checking
 *
 * This macro expands to a loop head.
 * Use it like:
 *
 *      int foo;
 *      ITERATE_ARGS_TYPE(it, args, foo, int) {
 *          // do stuff
 *      }
 */
#define ITERATE_ARGS_TYPE(it_, args_, varname_, type_) \
    for (it_ = (args_); \
         (ws_value_get_type(&it_->value) == WS_VALUE_TYPE_##type_ ) && \
         (((varname_) = ws_value_##type_##_get((struct ws_value_##type_ *) \
                                               it_)), 1); \
         ++it_)

#endif // __WS_COMMAND_UTILS_H__

