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
 * @addtogroup action "Action manager"
 *
 * @{
 *
 * @copydoc doc_actionmanager
 */

#ifndef __WS_ACTION_MANAGER_H__
#define __WS_ACTION_MANAGER_H__

#include "util/attributes.h"

// forward declarations
struct ws_message;
struct ws_object;
struct ws_reply;
struct ws_string;

/**
 * Initialize the action manager
 *
 * @note Tries to get reference on the context object, returns -EBUSY if
 * reference could not be aquired.
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_action_manager_init(
    struct ws_object* context //!< context object to use
);

/**
 * Process a transaction
 *
 * @note Should be called with reference on the transaction message already
 * aquired.
 *
 * @return the reply caused by the transaction being processed
 */
struct ws_reply*
ws_action_manager_process(
    struct ws_message* message, //!< transaction to process
    struct ws_object* opt_ctx //!< optional additional context object
)
__ws_nonnull__(1)
;

/**
 * Register a transaction to be run on an event
 *
 * @note the transaction has to be registered first via
 *       `ws_action_manager_process()`
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_action_manager_register(
    struct ws_string* event_name, //!< event on which to invoke the transaction
    struct ws_string* transaction_name //!< transaction to invoke
)
__ws_nonnull__(1, 2)
;

/**
 * Stop running a transaction to be run on an event
 *
 * @note undos what `ws_action_manager_register()` does
 *
 * @note Should be called with already aquired ref on argument.
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_action_manager_unregister_event(
    struct ws_string* event_name //!< event which to remove
)
__ws_nonnull__(1)
;

/**
 * Removes a transaction from the transaction storage
 *
 * @note after removal, the transaction cannot be registered to be run on
 *       events any more, existing registrations are, however, not removed
 *
 * @note Should be called with already aquired ref on argument.
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_action_manager_unregister_transaction(
    struct ws_string* transaction_name //!< name of transaction to remove
)
__ws_nonnull__(1)
;


#endif // __WS_ACTION_MANAGER_H__

/**
 * @}
 */

/**
 * @}
 */

