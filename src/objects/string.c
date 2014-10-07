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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unicode/ustring.h>

#include "objects/string.h"

#include "objects/object.h"
#include "objects/array.h"

/*
 *
 *
 * forward declarations  
 *
 *
 */

static bool
deinit_callback(
    struct ws_object* const
);

/*
 *
 *
 * v-table
 *
 *
 */

ws_object_type_id WS_OBJECT_TYPE_ID_STRING = {
    .supertype = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr = "ws_string",
    
    .init_callback = NULL,
    .deinit_callback = deinit_callback,
    .log_callback = NULL,
    .run_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = (int (*) (struct ws_object const*, struct ws_object const*))
                    ws_string_cmp,
};

bool
ws_string_init(
    struct ws_string* self
) { 
    //!< @todo implement
    return false; 
}

struct ws_string*
ws_string_new(void)
{
    //!< @todo implement
    return NULL;
}

size_t
ws_string_len(
    struct ws_string* self
){
    //!< @todo implement
    return 0;
}

struct ws_string*
ws_string_cat(
    struct ws_string* self,
    struct ws_string* other
){
    //!< @todo implement
    return NULL;
}


struct ws_string*
ws_string_multicat(
    struct ws_string* self,
    struct ws_array* others
){
    //!< @todo implement
    return NULL; 
 }
 

struct ws_string*
ws_string_dupl(
    struct ws_string* self
){
    //!< @todo implement
    return NULL;
}

int
ws_string_cmp(
    struct ws_string* self,
    struct ws_string* other
){
    //!< @todo implement
    return 0;
}

int
ws_string_ncmp(
    struct ws_string* self,
    struct ws_string* other,
    size_t offset,
    size_t n
){
    //!< @todo implement
    return 0;
}

bool
ws_string_substr(
    struct ws_string* self,
    struct ws_string* other
){
    //!< @todo implement
    return false;
}

char*
ws_string_raw(
    struct ws_string* self
){
    //!< @todo implement
    return NULL;   
}

/*
 *
 * Static Function Implementations
 *
 */
static bool
deinit_callback(
    struct ws_object* const self
) {
    //!< @todo implement
    return true;
}
