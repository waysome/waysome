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
 * @addtogroup serializer_module "Serializer module"
 *
 * @{
 */

/**
 * @addtogroup serializer_module_json_backend "Serializer JSON backend"
 *
 * @{
 */

/**
 * @addtogroup serializer_module_json_backend_deser "JSON backend deserializer"
 *
 * Serializer module JSON backend private utilities
 *
 * @{
 */

#ifndef __WS_SERIALIZE_JSON_DESERIALIZER_CALLBACKS_H__
#define __WS_SERIALIZE_JSON_DESERIALIZER_CALLBACKS_H__

/**
 * YAJL callback: null
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_null_cb(
    void * ctx
);

/**
 * YAJL callback: boolean
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_boolean_cb(
    void * ctx,
    int b
);

/**
 * YAJL callback: integer
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_integer_cb(
    void * ctx,
    long long i
);

/**
 * YAJL callback: double
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_double_cb(
    void * ctx,
    double d
);

/**
 * YAJL callback: string
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_string_cb(
    void * ctx,
    const unsigned char * str,
    size_t len
);

/**
 * YAJL callback: start_map
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_start_map_cb(
    void * ctx
);

/**
 * YAJL callback: map_key
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_map_key_cb(
    void * ctx,
    const unsigned char * key,
    size_t len
);

/**
 * YAJL callback: end_map
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_end_map_cb(
    void * ctx
);

/**
 * YAJL callback: start_array
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_start_array_cb(
    void * ctx
);

/**
 * YAJL callback: end_array
 *
 * @return zero on failure, else nonzero value
 */
int
yajl_end_array_cb(
    void * ctx
);

#endif //__WS_SERIALIZE_JSON_DESERIALIZER_CALLBACKS_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

