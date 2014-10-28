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

#include <errno.h>
#include <malloc.h>
#include <string.h>

#include "action/processor_stack.h"
#include "values/nil.h"
#include "values/union.h"
#include "values/value.h"
#include "values/value_type.h"


#define INITIAL_STACK_SIZE (4)


int
ws_processor_stack_init(
    struct ws_processor_stack* self
) {
    self->data = calloc(1, sizeof(*(self->data)) * INITIAL_STACK_SIZE);
    if (!self->data) {
        return -ENOMEM;
    }

    self->size = INITIAL_STACK_SIZE;
    self->top  = 0;

    return 0;
}

void
ws_processor_stack_deinit(
    struct ws_processor_stack* self
) {
    union ws_value_union* cur = self->data +  self->top;
    while (cur > self->data) {
        --cur;
        ws_value_deinit(&cur->value);
    }

    free(self->data);
}

int
ws_processor_stack_push(
    struct ws_processor_stack* self,
    size_t slots
) {
    // calculate the new `top` and, assuming a resize, calculate the new size
    size_t new_size = self->size;
    size_t new_top  = self->top + slots;
    while ((new_top + 1) > new_size) {
        new_size *= 2;
    }

    // check whether we have to resize
    if (new_size != self->size) {
        // try to reallocate
        union ws_value_union* new_data;
        new_data = realloc(self->data, sizeof(*(self->data)) * new_size);
        if (!new_data) {
            return -ENOMEM;
        }

        // initialize all the things we have to initialize
        self->data = new_data;
        self->size = new_size;
        memset(new_data + new_top, 0,
               sizeof(*(self->data)) * (new_size - new_top));
    }

    // set the new top
    self->top = new_top;

    // initialize all the values pushed
    size_t old_top = self->top;
    while (new_top > old_top) {
        --new_top;
        ws_value_nil_init(&self->data[new_top].nil);
    }

    return 0;
}

int
ws_processor_stack_pop(
    struct ws_processor_stack* self,
    size_t slots
) {
    // check whether there are at least that many values on the stack
    if (slots > (self->top - self->frame)) {
        return -EINVAL;
    }

    // calculate the new `top` and, assuming a resize, calculate the new size
    size_t new_size = self->size;
    size_t new_top  = self->top - slots;
    while (((new_top + 1) > new_size) && ((new_top + 1) > INITIAL_STACK_SIZE)) {
        new_size /= 2;
    }

    // deinitialize all the values on the way down
    size_t cur = self->top;
    while (cur > new_top) {
        --cur;
        ws_value_deinit(&self->data[cur].value);
        self->data[cur].value.type = WS_VALUE_TYPE_NONE;
    }

    memset(self->data + new_top, 0, self->top - new_top);

    // set the top
    self->top = new_top;

    // check whether we may resize
    if (new_size != self->size) {
        // try to reallocate
        union ws_value_union* new_data;
        new_data = realloc(self->data, sizeof(*(self->data)) * new_size);
        if (!new_data) {
            // noncritical error
            return 0;
        }

        self->data = new_data;
        self->size = new_size;
    }

    return 0;
}

union ws_value_union*
ws_processor_stack_top(
    struct ws_processor_stack* self
) {
    if (!self->data) {
        return NULL;
    }

    return self->data + self->top;
}

union ws_value_union*
ws_processor_stack_bottom(
    struct ws_processor_stack* self
) {
    if (!self->top) {
        return NULL;
    }

    return self->data;
}

struct ws_value*
ws_processor_stack_value_at(
    struct ws_processor_stack* self,
    ssize_t pos
) {
    // determine whether pos of from top or bottom and calculate
    if (pos >= 0) {
        // check whether the position is valid
        if ((size_t) pos >= self->top) {
            return NULL;
        }

        return &(self->data + pos)->value;
    }

    // check whether the position is valid
    if ((size_t) (-pos) > self->top) {
        return NULL;
    }
    return &(self->data + self->top + pos)->value;
}

size_t
ws_processor_stack_start_frame(
    struct ws_processor_stack* self
) {
    size_t old_frame = self->frame;

    // the new frame base is at the top. The frame is empty
    self->frame = self->top;

    return old_frame;
}

int
ws_processor_stack_restore_frame(
    struct ws_processor_stack* self,
    size_t frame_handle
) {
    // perform a sanity check: we can only restore old frames
    if (frame_handle > self->frame) {
        return -EINVAL;
    }

    // remove all the elements from the old frame
    ws_processor_stack_pop(self, self->top - self->frame);

    // restore the frame
    self->frame = frame_handle;

    return 0;
}

