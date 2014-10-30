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

#ifndef __WS_ACTION_MANAGER_H__
#define __WS_ACTION_MANAGER_H__

#include "util/attributes.h"

// forward declarations
struct ws_message;
struct ws_reply;

/**
 * Initialize the action manager
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_action_manager_init(void);

/**
 * Deinit the action manager
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_action_manager_deinit(void);

/**
 * Process a transaction
 *
 * @return the reply caused by the transaction being processed
 */
struct ws_reply*
ws_action_manager_process(
    struct ws_message* message //!< transaction to process
)
__ws_nonnull__(1)
;


#endif // __WS_ACTION_MANAGER_H__
