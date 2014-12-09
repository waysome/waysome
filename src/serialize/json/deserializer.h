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
 * @addtogroup serializer "Serializer"
 *
 * @{
 */

/**
 * @addtogroup serializer_json "Serializer JSON backend"
 *
 * @{
 */

/**
 * @addtogroup serializer_json_deserializer "JSON backend deserializer"
 *
 * @{
 */

#ifndef __WS_SERIALIZE_JSON_DESERIALIZER_H__
#define __WS_SERIALIZE_JSON_DESERIALIZER_H__

/**
 * Get a new deserializer object
 *
 * @return new deserializer object or NULL on failure
 */
struct ws_deserializer*
ws_serializer_json_deserializer_new(void);

#endif //__WS_SERIALIZE_JSON_DESERIALIZER_H__


/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
