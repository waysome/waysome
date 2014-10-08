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

#ifndef __WS_BUFFER_H__
#define __WS_BUFFER_H__

#include <stdint.h>

#include "objects/wayland_obj.h"
#include "util/attributes.h"

/*
 *
 * Forward declarations
 *
 */

struct ws_buffer;


/*
 *
 * Interface
 *
 */

/**
 * Callbacks for buffer operations
 */
struct ws_buffer_type {
    struct ws_object_type type; //!< regular object type
    void*       (*get_data)(struct ws_buffer*);
    int32_t     (*get_width)(struct ws_buffer*);
    int32_t     (*get_height)(struct ws_buffer*);
    int32_t     (*get_stride)(struct ws_buffer*);
    uint32_t    (*get_format)(struct ws_buffer*);
    void        (*begin_access)(struct ws_buffer*);
    void        (*end_access)(struct ws_buffer*);
};

typedef struct ws_buffer_type const ws_buffer_type_id;

/**
 * Abstract buffer type
 */
struct ws_buffer {
    struct ws_object obj;
    struct ws_buffer_cbs* funcs;
};

/**
 * Variable which holds the type information about the ws_wayland_obj type
 */
extern ws_buffer_type_id WS_OBJECT_TYPE_ID_BUFFER;

/**
 * Initialize a `ws_bufer` object
 *
 * @return zero on success, else negative errno.h constant
 */
int
ws_buffer_init(
    struct ws_buffer* self //!< The buffer to initialize
);

/**
 * Get a pointer to the buffer's contents
 *
 * @warning to not pass NULL to this function! It will crash!
 *
 * @return pointer to the buffer's contents
 */
void*
ws_buffer_data(
    struct ws_buffer* self //!< The buffer to query
)
__ws_nonnull__(1)
;

/**
 * Get the buffer's width
 *
 * @warning to not pass NULL to this function! It will crash!
 *
 * @return width of the buffer's contents
 */
int32_t
ws_buffer_width(
    struct ws_buffer* self //!< The buffer to query
)
__ws_nonnull__(1)
;

/**
 * Get the buffer's height
 *
 * @warning to not pass NULL to this function! It will crash!
 *
 * @return height of the buffer's contents
 */
int32_t
ws_buffer_height(
    struct ws_buffer* self //!< The buffer to query
)
__ws_nonnull__(1)
;

/**
 * Get the buffer's stride
 *
 * @warning to not pass NULL to this function! It will crash!
 *
 * @return stride of the buffer's contents
 */
int32_t
ws_buffer_stride(
    struct ws_buffer* self //!< The buffer to query
)
__ws_nonnull__(1)
;

/**
 * Get the buffer's format
 *
 * @warning to not pass NULL to this function! It will crash!
 *
 * @return format of the buffer's contents
 */
uint32_t
ws_buffer_format(
    struct ws_buffer* self //!< The buffer to query
)
__ws_nonnull__(1)
;
/**
 * Begin buffer access
 *
 * @warning to not pass NULL to this function! It will crash!
 */
void
ws_buffer_begin_access(
    struct ws_buffer* self //!< The buffer to begin a transaction on
)
__ws_nonnull__(1)
;
/**
 * End buffer access
 *
 * @warning to not pass NULL to this function! It will crash!
 */
void
ws_buffer_end_access(
    struct ws_buffer* self //!< The buffer to end a transaction on
)
__ws_nonnull__(1)
;

#endif // __WS_BUFFER_H__

