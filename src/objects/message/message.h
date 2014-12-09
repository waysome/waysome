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
 * @addtogroup objects "Classes"
 *
 * @{
 */

/**
 * @addtogroup objects_messages "Message classes"
 *
 * @{
 */

#ifndef __WS_OBJECTS_MESSAGE_H__
#define __WS_OBJECTS_MESSAGE_H__

#include <malloc.h>

#include "objects/object.h"

/**
 * Message type
 *
 * Objects of this type represent messages both received and send to and fro
 * modules.
 *
 * @extends ws_object
 */
struct ws_message {
    struct ws_object obj; //!< @protected parent object
    size_t id; //!< @protected id of the message
};

/**
 * Variable which holds the type information about the ws_message type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_MESSAGE;

/**
 * Initialize a ws_message
 *
 * @note for use by the derived types
 *
 * @return 0 on success, or a negative error code
 */
int
ws_message_init(
    struct ws_message* self, //!< message to initialize
    size_t id //!< id to initialize the message with
)
__ws_nonnull__(1)
;

/**
 * Get id of message object
 *
 * @memberof ws_message
 *
 * @return id of the message
 */
size_t
ws_message_get_id(
    struct ws_message* self //!< message to initialize
)
__ws_nonnull__(1)
;

#endif //__WS_OBJECTS_MESSAGE_H__

/**
 * @}
 */

/**
 * @}
 */

