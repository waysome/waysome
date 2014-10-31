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

#ifndef __WS_INPUT_HOTKEY_EVENT_H__
#define __WS_INPUT_HOTKEY_EVENT_H__

#include <stdint.h>

#include "objects/object.h"
#include "objects/string.h"


/**
 * A hotkey event
 */
struct ws_hotkey_event {
    struct ws_object obj; //!< @public supertype
    struct ws_string name; //!< @public name of event to emit
    uint16_t* codes; //!< @public codes
    uint16_t code_num; //!< @public number of codes
};

/**
 * Variable which holds type information about the ws_hotkey_event type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_HOTKEY_EVENT;

#endif // __WS_INPUT_HOTKEY_EVENT_H__

