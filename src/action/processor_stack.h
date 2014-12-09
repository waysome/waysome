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
 */

/**
 * @addtogroup action_processor "Action manager processor"
 *
 * @{
 */

/**
 * @addtogroup action_processor_stack "Action manager processor stack"
 *
 * @{
 */

#ifndef __WS_ACTION_STACK_H__
#define __WS_ACTION_STACK_H__

#include <stddef.h>
#include <malloc.h>

#include "util/attributes.h"

// forward declarations
struct ws_value;
union ws_value_union;

/**
 * A processor's stack
 *
 * This type represents a stack which may be used for executing commands.
 *
 * @see stack_semantics
 */
struct ws_processor_stack {
    union ws_value_union* data; //!< @private basepointer of the stack
    size_t size; //!< @private size of the stack
    size_t top; //!< @private top of the stack, as position from the basepointer
    size_t frame; //!< @private base pointer of the current frame
};

/**
 * Initialize a processor stack
 *
 * Initialize the processor stack with a default height
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_processor_stack_init(
    struct ws_processor_stack* self //!< stack to initialize
)
__ws_nonnull__(1)
;

/**
 * Deinitialize a processor stack
 */
void
ws_processor_stack_deinit(
    struct ws_processor_stack* self //!< stack to deinitialize
)
__ws_nonnull__(1)
;

/**
 * Push values on the stack
 *
 * This function increases the top counter, initializing the values to
 * nil-values.
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_processor_stack_push(
    struct ws_processor_stack* self, //!< the stack
    size_t slots //!< number of slots to push
)
__ws_nonnull__(1)
;

/**
 * Pops from the stack
 *
 * This function decreases the top counter, deinitializing and invalidating the
 * values popped.
 *
 * @return 0 if the operation succeeded, a negative error number otherwise
 */
int
ws_processor_stack_pop(
    struct ws_processor_stack* self, //!< the stack
    size_t slots //!< number of slots to pop
)
__ws_nonnull__(1)
;

/**
 * Get the top of the stack
 *
 * This function returns the top of the stack, which actually is an invalid
 * value.
 * If you subtract from that pointer, you get valid values _on_ the stack, so
 * `-1`, for example, is the topmost element.
 * At some point, you reach the bottom of the stack.
 *
 * @return pointer to the top of the stack
 */
union ws_value_union*
ws_processor_stack_top(
    struct ws_processor_stack* self //!< the stack
)
__ws_nonnull__(1)
;

/**
 * Get the bottom of the stack
 *
 * This function returns the bottom of the stack.
 * If you add to that pointer, you move toward the top of the stack.
 *
 * @return pointer to the bottom of the stack
 */
union ws_value_union*
ws_processor_stack_bottom(
    struct ws_processor_stack* self //!< the stack
)
__ws_nonnull__(1)
;

/**
 * Get a value at a given position
 *
 * @return value at given position
 */
struct ws_value*
ws_processor_stack_value_at(
    struct ws_processor_stack* self, //!< the stack
    ssize_t pos, //!< position of value
    struct ws_value* top //!< top to use (`NULL` for internal top)
)
__ws_nonnull__(1)
;

/**
 * Start a frame at the current position
 *
 * @return handle with which one may reset the frame
 */
size_t
ws_processor_stack_start_frame(
    struct ws_processor_stack* self //!< the stack
)
__ws_nonnull__(1)
;

/**
 * Restore a frame
 *
 * Call this function to restore a frame previously started using
 * `ws_processor_start_frame()`.
 *
 * @return 0 on success, a negative error number otherwise
 */
int
ws_processor_stack_restore_frame(
    struct ws_processor_stack* self, //!< the stack
    size_t frame_handle //!< handle returned by `ws_processor_start_frame()`
)
__ws_nonnull__(1)
;

#endif // __WS_ACTION_STACK_H__

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

