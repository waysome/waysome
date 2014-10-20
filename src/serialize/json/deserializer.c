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
 * @file deserializer.c
 *
 * This file contains the deserializer backend for JSON. For a full
 * documentation of the statemachine @ref json_statemachine "see here".
 *
 * The implementation makes use of the yajl library. For a documentation of the
 * yajl library, see: https://lloyd.github.io/yajl/
 */

#include <string.h>
#include <stdlib.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "serialize/deserializer.h"
#include "serialize/json/common.h"
#include "serialize/json/deserializer.h"
#include "serialize/json/deserializer_state.h"

#include "logger/module.h"

static struct ws_logger_context log_ctx = {
    .prefix = "[JSON Deserializer] ",
};

/*
 *
 * static function declarations
 *
 */

/**
 * deserialize callback
 *
 * @return negative error value on error, else number of deserialized bytes
 */
static ssize_t
deserialize(
    struct ws_deserializer* self,
    char const* buf,
    size_t nbuf
);

/*
 *
 * variables
 *
 */

//!< @todo implement yajl callbacks

static yajl_callbacks YAJL_CALLBACKS = {
//    .yajl_null          = yajl_null_cb,
    .yajl_null          = NULL,
//    .yajl_boolean       = yajl_boolean_cb,
    .yajl_boolean       = NULL,
//    .yajl_integer       = yajl_integer_cb,
    .yajl_integer       = NULL,
//    .yajl_double        = yajl_double_cb,
    .yajl_double        = NULL,
//    .yajl_number        = yajl_number_cb,
    .yajl_number        = NULL,
//    .yajl_string        = yajl_string_cb,
    .yajl_string        = NULL,
//    .yajl_start_map     = yajl_start_map_cb,
    .yajl_start_map     = NULL,
//    .yajl_map_key       = yajl_map_key_cb,
    .yajl_map_key       = NULL,
//    .yajl_end_map       = yajl_end_map_cb,
    .yajl_end_map       = NULL,
//    .yajl_start_array   = yajl_start_array_cb,
    .yajl_start_array   = NULL,
//    .yajl_end_array     = yajl_end_array_cb,
};

/*
 *
 * interface implementation
 *
 */

struct ws_deserializer*
ws_serializer_json_deserializer_new(void)
{
    //!< @todo implement
    return NULL;
}

/*
 *
 * static function implementations
 *
 */

static ssize_t
deserialize(
    struct ws_deserializer* self,
    char const* buf,
    size_t nbuf
) {
    //!< @todo implement
    return 0;
}
