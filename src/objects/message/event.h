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

#ifndef __WS_OBJECTS_EVENT_H__
#define __WS_OBJECTS_EVENT_H__

#include "objects/message/message.h"
#include "objects/string.h"
#include "values/union.h"

/**
 * Event type
 *
 * @extends ws_message
 */
struct ws_event {
    struct ws_message m; //!< @protected Base class
    struct ws_string name; //!< @protected Name of the event
    union ws_value_union context; //!< @protected Value holding some context
};

/**
 * Variable which holds the type information about the ws_event type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_EVENT;

/**
 * Initialize a ws_event
 *
 * @memberof ws_event
 *
 * @return 0 on success, or a negative error code
 */
int
ws_event_init(
    struct ws_event* self, //!< message to initialize
    struct ws_string* name, //!< Name for the event
    struct ws_value* ctx //!< Context to attach, can be NULL
)
__ws_nonnull__(1)
;

/**
 * Get the name of an event
 *
 * @memberof ws_event
 *
 * @return The name of the event or NULL on failure
 */
struct ws_string*
ws_event_get_name(
    struct ws_event* self
)
__ws_nonnull__(1)
;

/**
 * Get the context of an event
 *
 * @memberof ws_event
 *
 * @return The name of the event or NULL on failure
 */
union ws_value_union*
ws_event_get_context(
    struct ws_event* self
)
__ws_nonnull__(1)
;

#endif //__WS_OBJECTS_EVENT_H__

