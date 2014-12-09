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

#ifndef __WS_OBJECTS_TRANSACTION_H__
#define __WS_OBJECTS_TRANSACTION_H__

#include <stdbool.h>
#include <stdint.h>

#include "command/command.h"
#include "objects/message/message.h"
#include "objects/string.h"

/**
 * Transaction action type
 *
 * Identifier for what should be done with a transaction
 */
enum ws_transaction_flags {
    WS_TRANSACTION_FLAGS_REGISTER   = 1 << 0,
    WS_TRANSACTION_FLAGS_EXEC       = 1 << 1,
};

/**
 * Type for command list
 */
struct ws_transaction_command_list {
    size_t len; //!< @protected length of the command array
    size_t num; //!< @protected next free position/number of statements
    struct ws_statement* statements; //!< @protected Transaction statements
};

/**
 * Transaction type
 *
 * @extends ws_message
 */
struct ws_transaction {
    struct ws_message m; //!< @protected Base class.

    uintmax_t connection_id; //!< @protected Connection id

    struct ws_string* name; //!< @protected In case of Register/Store: Name
    enum ws_transaction_flags flags; //!< @protected What should be done?

    struct ws_transaction_command_list* cmds; //!< @protected Commands
};

extern ws_object_type_id WS_OBJECT_TYPE_ID_TRANSACTION;

/**
 * Initialize a `ws_transaction` object
 *
 * @note Gets a ref on the name object for you
 *
 * @return 0 if the initialization was successful, a negative error number
 *         otherwise
 */
int
ws_transaction_init(
    struct ws_transaction* self, //!< transaction to initialize
    size_t id, //!< id to initialize the message with
    struct ws_string* name //!< Name of the transaction
)
__ws_nonnull__(1, 3)
;

/**
 * Get a new `ws_transaction` object
 *
 * @note Gets a ref on the name object for you
 *
 * @return New transaction object or NULL on failure
 */
struct ws_transaction*
ws_transaction_new(
    size_t id, //!< id to initialize the message with
    struct ws_string* name, //!< Name of the transaction
    enum ws_transaction_flags flags, //!< Flags for the transaction
    struct ws_transaction_command_list* cmds //!< Commands for the transaction
);

/**
 * Get the id of the connection for the transaction
 *
 * @return Connection id for the connection of the transaction
 */
uintmax_t
ws_transaction_connection_id(
    struct ws_transaction* t //!< The transaction
);

/**
 * Set the id of the connection for the transaction
 */
void
ws_transaction_set_connection_id(
    struct ws_transaction* t, //!< The transaction
    uintmax_t connection_id //!< The new connection id
);

/**
 * Get the flags of the transaction
 *
 * @return Flags of the transaction
 */
enum ws_transaction_flags
ws_transaction_flags(
    struct ws_transaction* t //!< The transaction
);

/**
 * Set the flags of the transaction
 *
 * @warning Hard-sets the flags, does _not_ set single bits
 */
void
ws_transaction_set_flags(
    struct ws_transaction* t, //!< The transaction
    enum ws_transaction_flags flags //!< Flags to set
);

/**
 * Get the name of the transaction
 *
 * @note Gets a ref on the name string object if it exists
 *
 * @return Name of the transaction
 */
struct ws_string*
ws_transaction_name(
    struct ws_transaction* t //!< The transaction
);

/**
 * Set the name of the transaction
 *
 * @note Gets a ref on the name string object if it exists
 */
void
ws_transaction_set_name(
    struct ws_transaction* t, //!< The transaction
    struct ws_string* name //!< The name
);

/**
 * Get the command list of the transaction
 *
 * @return list of commands of the transaction
 */
struct ws_transaction_command_list*
ws_transaction_commands(
    struct ws_transaction* t //!< The transaction
);

/**
 * Append a statement to the transaction
 *
 * @note The passed statement can be free()'d afterwards.
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_transaction_push_statement(
    struct ws_transaction* t, //!< The transaction
    struct ws_statement* statement //!< The statement to add
);

#endif //__WS_OBJECTS_TRANSACTION_H__

/**
 * @}
 */

/**
 * @}
 */

