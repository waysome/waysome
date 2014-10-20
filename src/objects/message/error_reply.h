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

#ifndef __WS_OBJECTS_ERROR_REPLY_H__
#define __WS_OBJECTS_ERROR_REPLY_H__

#include "objects/message/reply.h"


/**
 * Error reply message type
 *
 * This reply message type is intended for communication of errors back to the
 * issuer of a transaction.
 */
struct ws_error_reply {
    struct ws_reply reply; //!< @protected base class
    unsigned int code; //!< error code (e.g. an errno.h value)
    char const* description; //!< error description
    char const* cause; //!< cause of the error
};

/**
 * Variable which holds the type information about the ws_error_reply type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_ERROR_REPLY;

#endif // __WS_OBJECTS_ERROR_REPLY_H__

