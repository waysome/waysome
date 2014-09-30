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

#include "compositor/module.h"

#include <stdbool.h>

#include "util/cleaner.h"


/**
 * Internal compositor context
 * 
 * This context holds the internal state of the compositor.
 */
static struct {
} comp_ctx;


/**
 * Deinitialise the compositor
 *
 * This function deinitialises the compositor.
 * It does not take care of inter-module dependency nor does it check whether
 * it was called before, unlike ws_compositor_init().
 * This function should only be called from the main function itself.
 *
 */
static void
ws_compositor_deinit(
    void* dummy
);


/*
 *
 * Interface implementation
 *
 */

int
ws_compositor_init(void) {
    static bool is_init = false;
    if (is_init) {
        return 0;
    }

    //!< @todo: open framebuffer device by hardcoded path

    //!< @todo: get all the connectors

    //!< @todo: create a framebuffer for each connector

    //!< @todo: prelimary: preload a PNG from a hardcoded path

    //!< @todo: prelimary: blit the preloaded PNG on each of the frame buffers

    is_init = true;
    return -1;
}

/*
 *
 * Internal implementation
 *
 */


static void
ws_compositor_deinit(
    void* dummy
) {
    //!< @todo: free all of the framebuffers

    //!< @todo: prelimary: free the preloaded PNG
}

