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
 * @addtogroup objects_queue "Class: queue"
 *
 * @{
 */

#ifndef __WS_OBJECTS_QUEUE_H__
#define __WS_OBJECTS_QUEUE_H__

#include <stdbool.h>
#include <wayland-util.h>

#include "objects/object.h"

/**
 * Queue type
 */
struct ws_queue {
    struct ws_object obj; //!< @protected Base class
    struct wl_list link; //!< @private Actual queue
};


/**
 * Variable which holds type information about the ws_queue type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_QUEUE;

/**
 * Initialize a queue object
 *
 * @memberof ws_queue
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_queue_init(
    struct ws_queue* q
);

/**
 * Allocate a new queue
 *
 * @memberof ws_queue
 *
 * @return new ws_queue object or NULL on failure
 */
struct ws_queue*
ws_queue_new(void);

/**
 * Get the next element from the queue
 *
 * @memberof ws_queue
 *
 * @note Removes the element from the queue, does _NOT_ unref it, so this is a
 * move operation.
 *
 * @return next ws_object in queue or NULL on failure (or queue empty)
 */
struct ws_object*
ws_queue_pop(
    struct ws_queue* q
);

/**
 * Push an element to the queue
 *
 * @memberof ws_queue
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_queue_push(
    struct ws_queue* q,
    struct ws_object* o
);

/**
 * Check whether the queue is empty
 *
 * @memberof ws_queue
 *
 * @return true if queue is empty, false otherwise
 */
bool
ws_queue_empty(
    struct ws_queue* q
);

#endif // __WS_OBJECTS_QUEUE_H__

/**
 * @}
 */

/**
 * @}
 */
