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

#ifndef __WS_OBJECTS_STRING_H__
#define __WS_OBJECTS_STRING_H__

#include <stdbool.h>
#include <unicode/ustring.h>

#include "objects/object.h"
#include "objects/array.h"

/**
* ws_string type definition
 *
 * @extends ws_object
*/
struct ws_string {
    struct ws_object obj; //!< @protected Base class.
    size_t charcount; //!< @protected Number of characters in the string
    UChar* str; //!< @protected UChar array containing the actual string
    bool is_utf8;
};

/**
* ws_string type information
*/
extern ws_object_type_id WS_OBJECT_TYPE_ID_STRING;

/**
 * Initialize a ws_string
 *
 * @memberof ws_string
 *
 * @return true if ws_string object was successfully initialized,
 * NULL on failire
 */
bool
ws_string_init(
    struct ws_string* self
);

/**
 * Allocate a new, initialized ws_string
 *
 * @memberof ws_string
 *
 * @return allocated, initialized ws_string object, NULL on failure
 */
struct ws_string*
ws_string_new(void);

/**
 * Get length of a ws_string
 *
 * @memberof ws_string
 *
 * @return length of ws_string, 0 on NULL passed
 */
size_t
ws_string_len(
    struct ws_string* self
);

/**
 * Concatenate two ws_strings
 *
 * @memberof ws_string
 *
 * @return self with other appended to self, NULL on failure
 */
struct ws_string*
ws_string_cat(
    struct ws_string* self,
    struct ws_string* other
);

/**
 * Concatenate multiple ws_strings
 *
 * @memberof ws_string
 *
 * @return ws_string object with contents of other consecutively appended
 * to self, NULL if ws_array contains at least one non-string element or is NULL */

struct ws_string*
ws_string_multicat(
    struct ws_string* self,
    struct ws_array* others
);


/**
 * Duplicate a ws_string object
 *
 * @memberof ws_string
 *
 * @return copy of self, NULL on failure
 */
struct ws_string*
ws_string_dupl(
    struct ws_string* self
);

/**
 * Compare if the contents of two ws_strings are equal
 *
 * @memberof ws_string
 *
 * @return 0 if the contents of both strings are equal,
 * -1 if the first character that does not match has a lower value in self than
 * in other, 1 if the first character that does not match has a greater value in
 * self than in other
 */
int
ws_string_cmp(
    struct ws_string* self,
    struct ws_string* other
);

/**
 * Compare if a substring of a ws_string and another ws_string are equal
 *
 * @memberof ws_string
 *
 * @return 0 if the contents of self's substring and other are equal,
 * -1 if the first character that does not match has a lower value in self than
 * in other, 1 if the first character that does not match has a greater value in
 * self than in other
 */
int
ws_string_ncmp(
    struct ws_string* self,
    struct ws_string* other,
    size_t offset,
    size_t n
);

/**
 * Checks if a string is a substring of another ws_string
 *
 * @memberof ws_string
 *
 * @return true if other is a substring, else false
 */
bool
ws_string_substr(
    struct ws_string* self,
    struct ws_string* other
);

/**
 * Get ws_string as a UTF-8 string
 *
 * @memberof ws_string
 *
 * @warning returns NULL if ws_string is no UTF8 string
 *
 * @return Returns the ws_string as an UTF-8 string, NULL on failure
 */
char*
ws_string_raw(
    struct ws_string* self
);

/**
 * Set the string contained in a ws_string object to the passed UTF8 string
 *
 * @memberof ws_string
 */
void
ws_string_set_from_raw(
    struct ws_string* self,
    char* raw
);


#endif // __WS_OBJECTS_STRING_H__

/**
 * @}
 */
