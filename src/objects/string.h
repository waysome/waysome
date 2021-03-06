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
 * @addtogroup objects_string "Class: string type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_STRING_H__
#define __WS_OBJECTS_STRING_H__

#include <stdbool.h>
#include <unicode/ustring.h>

#include "objects/object.h"

/**
 * ws_string type definition
 *
 * @extends ws_object
*/
struct ws_string {
    struct ws_object obj; //!< @protected Base class.
    UChar* str; //!< @protected UChar array containing the actual string
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
 * false on failure
 */
bool
ws_string_init(
    struct ws_string* self
);

/**
 * Sets a ws_string's attributes from another ws_string
 *
 * @memberof ws_string
 *
 * @return true if ws_string object was successfully set,
 * false on failure
 */
bool
ws_string_set_from_str(
    struct ws_string* self,
    struct ws_string* other
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
 * Get length of a ws_string in number of characters
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
 * @warning Returned string is _newly allocated_
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
 *
 * @return 0 on success, else negative errno number
 */
int
ws_string_set_from_raw(
    struct ws_string* self,
    const char* raw
);


#endif // __WS_OBJECTS_STRING_H__

/**
 * @}
 */

/**
 * @}
 */

