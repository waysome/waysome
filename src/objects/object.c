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
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "logger/module.h"
#include "objects/object.h"
#include "util/condition.h"
#include "util/string.h"
#include "values/bool.h"
#include "values/int.h"
#include "values/nil.h"
#include "values/object_id.h"
#include "values/set.h"
#include "values/string.h"

static struct ws_logger_context log_ctx = {
    .prefix = "[Object] ",
};

/*
 *
 * Forward declarations
 *
 */

/*
 * Attribute information about the type
 */
struct ws_object_attribute const WS_OBJECT_ATTRS_OBJECT[] = {
/* The following code is outcommented as it should be an example only: */
//    {
//        .name = "id",
//        .offset_in_struct = offsetof(struct ws_object, id),// stddef: offsetof
//        .type = WS_OBJ_ATTR_NO_TYPE,
//    }
    {
        .name = NULL,
        .offset_in_struct = 0,
        .type = 0
    },
};

/*
 * Type information
 */
ws_object_type_id WS_OBJECT_TYPE_ID_OBJECT = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_object",

    .deinit_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = NULL,
    .uuid_callback = NULL,
    .attribute_table = WS_OBJECT_ATTRS_OBJECT,
    .function_table = NULL,
};

static const struct {
    enum ws_value_type type;
} ATTR_TYPE_VALUE_TYPE_MAP[] = {
//    [WS_OBJ_ATTR_TYPE_CHAR]     = {
//        .type = WS_VALUE_TYPE_STRING,
//    },

    [WS_OBJ_ATTR_TYPE_INT32]    = {
        .type = WS_VALUE_TYPE_INT,
    },

    [WS_OBJ_ATTR_TYPE_INT64]    = {
        .type = WS_VALUE_TYPE_INT,
    },

    [WS_OBJ_ATTR_TYPE_UINT32]   = {
        .type = WS_VALUE_TYPE_INT,
    },

    [WS_OBJ_ATTR_TYPE_UINT64]   = {
        .type = WS_VALUE_TYPE_INT,
    },

    [WS_OBJ_ATTR_TYPE_DOUBLE]   = {
        .type = WS_VALUE_TYPE_INT,
    }, //!< @todo double?

    [WS_OBJ_ATTR_TYPE_STRING]   = {
        .type = WS_VALUE_TYPE_STRING,
    },

    [WS_OBJ_ATTR_TYPE_OBJ]      = {
        .type = WS_VALUE_TYPE_OBJECT_ID,
    },

};

struct ws_object*
ws_object_new(
    size_t s
) {
    if (s < sizeof(struct ws_object)) {
        return NULL;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Allocating");

    struct ws_object* o = calloc(1, s);

    if (o) {
        ws_object_init(o);
        o->settings = WS_OBJECT_HEAPALLOCED;
    }

    return o;
}

struct ws_object*
ws_object_new_raw(void) {
    return ws_object_new(sizeof(struct ws_object));
}

ws_object_type_id*
ws_object_get_type_id(
    struct ws_object* const self
) {
    if (self) {
        ws_object_type_id* id;
        ws_object_lock_read(self);
        id = self->id;
        ws_object_unlock(self);
        return id;
    }
    return NULL;
}

enum ws_object_settings
ws_object_get_settings(
    struct ws_object* const self
) {
    if (self) {
        enum ws_object_settings s;
        ws_object_lock_read(self);
        s = self->settings;
        ws_object_unlock(self);
        return s;
    }

    return WS_OBJ_NO_SETTINGS;
}

void
ws_object_set_settings(
    struct ws_object* self,
    enum ws_object_settings settings
) {
    if (self) {
        ws_object_lock_write(self);
        self->settings = settings;
        ws_object_unlock(self);
    }
}

bool
ws_object_init(
    struct ws_object* self
) {
    if (self) {
        ws_log(&log_ctx, LOG_DEBUG, "Initializing: %p", self);

        self->settings = WS_OBJ_NO_SETTINGS;

        pthread_rwlock_init(&self->rw_lock, NULL);
        pthread_mutex_init(&self->ref_counting.lock, NULL);
        self->ref_counting.refcnt = 1;

        self->uuid = 0;

        self->id = &WS_OBJECT_TYPE_ID_OBJECT;

        return true;
    }

    return false;
}

struct ws_object*
ws_object_getref(
    struct ws_object* self
) {
    if (!(self && (self->settings & WS_OBJECT_HEAPALLOCED))) {
        return self;
    }

    if (pthread_mutex_lock(&self->ref_counting.lock) != 0) {
        return NULL;
    };
    self->ref_counting.refcnt++;
    pthread_mutex_unlock(&self->ref_counting.lock);

    return self;
}

void
ws_object_unref(
    struct ws_object* self
) {
    if (!(self && (self->settings & WS_OBJECT_HEAPALLOCED))) {
        return;
    }

    if (pthread_mutex_lock(&self->ref_counting.lock) != 0) {
        return;
    };
    self->ref_counting.refcnt--;

    if (self->ref_counting.refcnt) {
        pthread_mutex_unlock(&self->ref_counting.lock);
        return;
    }
    pthread_mutex_unlock(&self->ref_counting.lock);

    ws_object_deinit(self);
    free(self);
}

ssize_t
ws_object_hash(
    struct ws_object* self
) {
    if (!self) {
        return -EINVAL;
    }

    ws_log(&log_ctx, LOG_DEBUG, "Hashing: %p (%s)", self, self->id->typestr);

    ws_object_type_id* type = self->id;
    while (!type->hash_callback) {
        // we hit the basic object type, which is totally abstract
        if (type == &WS_OBJECT_TYPE_ID_OBJECT) {
            return -ENOTSUP;
        }

        type = type->supertype;
    }

    ws_object_lock_read(self);
    size_t hash = type->hash_callback(self);
    ws_object_unlock(self);

    return hash;
}

bool
ws_object_lock_read(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_rdlock(&self->rw_lock);
}

bool
ws_object_lock_write(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_wrlock(&self->rw_lock);
}

int
ws_object_lock_try_read(
    struct ws_object* self
) {
    return -pthread_rwlock_tryrdlock(&self->rw_lock);
}

int
ws_object_lock_try_write(
    struct ws_object* self
) {
    return -pthread_rwlock_trywrlock(&self->rw_lock);
}

bool
ws_object_unlock(
    struct ws_object* self
) {
    return 0 == pthread_rwlock_unlock(&self->rw_lock);
}

void
ws_object_deinit(
    struct ws_object* self
) {
    ws_object_lock_write(self);

    // traverse towards the root, deinitializing
    ws_object_type_id* type = self->id;
    while (type != &WS_OBJECT_TYPE_ID_OBJECT) {
        ws_log(&log_ctx, LOG_DEBUG, "Deinitializing: %p (%s)",
                self, type->typestr);

        if (type->deinit_callback) {
            type->deinit_callback(self);
        }
        type = type->supertype;
    }

    // destroy the locks
    pthread_mutex_destroy(&self->ref_counting.lock);
    pthread_rwlock_destroy(&self->rw_lock);
}

bool
ws_object_has_attr(
    struct ws_object* self,
    char const* ident
) {
    ws_object_lock_read(self);
    if (unlikely(!self->id || !self->id->attribute_table)) {
        ws_object_unlock(self);
        return false;
    }

    ws_object_type_id* curtype = self->id;

    while (curtype != &WS_OBJECT_TYPE_ID_OBJECT) {
        struct ws_object_attribute const* iter;
        for (iter = &self->id->attribute_table[0]; iter->name; iter++) {
            if (ws_streq(iter->name, ident)) {
                ws_object_unlock(self);
                return true;
            }
        }

        curtype = curtype->supertype;
    }

    ws_object_unlock(self);
    return false;
}

int
ws_object_attr_read(
    struct ws_object* self,
    char const* ident,
    struct ws_value* dest
) {
    if (unlikely(!self->id || !self->id->attribute_table)) {
        return -EINVAL;
    }


    ws_object_lock_read(self);

    bool found = false;
    size_t offset = 0;
    enum ws_object_attribute_type type = WS_OBJ_ATTR_NO_TYPE;

    struct ws_object_attribute const* iter;
    for (iter = &self->id->attribute_table[0]; iter->name && !found; iter++) {
        if (ws_streq(iter->name, ident)) {
            offset  = iter->offset_in_struct;
            type    = iter->type;
            found   = true;
        }
    }

    if (unlikely(!found)) {
        ws_object_unlock(self);
        return -ECANCELED;
    }

    if (unlikely(type & WS_OBJ_ATTR_NO_TYPE)) {
        ws_object_unlock(self);
        return -EFAULT;
    }

    if (unlikely(ATTR_TYPE_VALUE_TYPE_MAP[type].type !=
                ws_value_get_type(dest))) {
        /* Types not matching */
        ws_object_unlock(self);
        return -EINVAL;
    }

    void* member_pos = (void *) (((char *) self) + offset);
    switch (type) {
    case WS_OBJ_ATTR_TYPE_CHAR:
        ws_value_int_set((struct ws_value_int*) dest,
                         (char) *((int32_t*) member_pos));
        break;

    case WS_OBJ_ATTR_TYPE_INT32:
        ws_value_int_set((struct ws_value_int*) dest,
                         (intmax_t) *((int32_t*) member_pos));
        break;

    case WS_OBJ_ATTR_TYPE_INT64:
        ws_value_int_set((struct ws_value_int*) dest,
                         (intmax_t) *((int64_t*) member_pos));
        break;

    case WS_OBJ_ATTR_TYPE_UINT32:
        ws_value_int_set((struct ws_value_int*) dest,
                         (intmax_t) *((uint32_t*) member_pos));
        break;

    case WS_OBJ_ATTR_TYPE_UINT64:
        ws_value_int_set((struct ws_value_int*) dest,
                         (intmax_t) *((uint64_t*) member_pos));
        break;

    case WS_OBJ_ATTR_TYPE_DOUBLE:
        //!< @todo implement casting to value type
        break;

    case WS_OBJ_ATTR_TYPE_STRING:
        {
            struct ws_string* s = ws_string_new();
            if (unlikely(!s)) {
                return -ENOMEM;
            }

            ws_string_set_from_raw(s, *(char**) member_pos);

            ws_value_string_set_str((struct ws_value_string*) dest, s);
        }
        break;

    case WS_OBJ_ATTR_TYPE_OBJ:
        ws_value_object_id_set((struct ws_value_object_id*) dest,
                                *(struct ws_object**) member_pos);
        break;

    default:
        //!< @todo Something strange happened in this case, maybe implementation
        // missing here. What to do now?
        break;
    };

    ws_object_unlock(self);
    return 0;
}

enum ws_object_attribute_type
ws_object_attr_type(
    struct ws_object* self,
    char* ident
) {
    ws_object_lock_read(self);

    if (!self->id || !self->id->attribute_table) {
        goto err;
    }

    size_t i;
    for (i = 0; self->id->attribute_table[i].name != NULL; i++) {
        if (ws_streq(self->id->attribute_table[i].name, ident)) {
            ws_object_unlock(self);
            return self->id->attribute_table[i].type;
        }
    }

err:
    ws_object_unlock(self);
    return WS_OBJ_ATTR_NO_TYPE;
}

int
ws_object_cmp(
    struct ws_object const* o1,
    struct ws_object const* o2
) {
    ws_log(&log_ctx, LOG_DEBUG, "Comparing: %p <=> %p", o1, o2);

    if ((o1 == NULL) ^ (o2 == NULL)) {
        return (o1 != NULL) ? -1 : 1;
    }

    if (o1 == o2) {
        return 0;
    }

    while (1) {
        int try = ws_object_lock_try_read((struct ws_object*) o1);
        if (try != 0) {
            if (try == -EAGAIN || try == -EWOULDBLOCK) {
                continue;
            }
            return try;
        }

        try = ws_object_lock_try_read((struct ws_object*) o2);
        if (try != 0) {
            ws_object_unlock((struct ws_object*) o1);
            if (try == -EAGAIN || try == -EWOULDBLOCK) {
                continue;
            }
            return try;
        }
        break;
    }

    int res;

    if (o1->id != o2->id) {
        res = 42; // determined by fair dice roll
        goto out;
    }

    ws_object_type_id* type = o1->id;
    while (!type->cmp_callback) {
        // we hit the basic object type, which is totally abstract
        if (type == &WS_OBJECT_TYPE_ID_OBJECT) {
            res = 17; // because it's such a nice prime number
            goto out;
        }

        type = type->supertype;
    }

    res = type->cmp_callback(o1, o2);

out:
    ws_object_unlock((struct ws_object*) o2); // unlock, remove `const`
    ws_object_unlock((struct ws_object*) o1); // unlock, remove `const`
    return res;
}

uintmax_t
ws_object_uuid(
    struct ws_object const* self //!< The object
) {
    while (self->uuid == 0) {
        // This case is rare, so we must cast here
        struct ws_object* _self = (struct ws_object*) self;
        if (!ws_object_lock_try_write(_self)) {
            // check again if uuid is set
            continue;
        }

        ws_object_type_id* type = self->id;

        while (!type->uuid_callback) {
            if (type == &WS_OBJECT_TYPE_ID_OBJECT) {
                uintmax_t buff[(sizeof(uuid_t) / sizeof(uintmax_t)) + 1];
                uuid_generate((unsigned char*) buff);
                _self->uuid = buff[0];

                goto iter_next;
            }

            type = type->supertype;
        }

        _self->uuid = type->uuid_callback(_self);

iter_next:
        ws_object_unlock(_self);
    }

    return self->uuid;
}

bool
ws_object_is_instance_of(
    struct ws_object const* self,
    ws_object_type_id* type
) {
    ws_object_type_id* curtype = self->id;

    // iterate over all the base types
    while (curtype != &WS_OBJECT_TYPE_ID_OBJECT) {
        if (curtype == type) {
            // yay! found it!
            return true;
        }

        curtype = curtype->supertype;
    }

    // nothing found
    return type == &WS_OBJECT_TYPE_ID_OBJECT;
}

bool
ws_object_has_typename(
    struct ws_object* self,
    const char* type_name
) {
    if (unlikely(!self || !type_name)) {
        return false;
    }

    ws_object_lock_read(self);
    ws_object_type_id* curtype = self->id;
    ws_object_unlock(self);

    // iterate over all the base types
    while (curtype != &WS_OBJECT_TYPE_ID_OBJECT) {
        if (ws_streq(curtype->typestr, type_name)) {
            // yay! found it!
            return true;
        }

        curtype = curtype->supertype;
    }

    return false;
}

int
ws_object_call_cmd(
    struct ws_object* self,
    char const* ident,
    union ws_value_union* stack
) {
    if (!self || !ident || !stack) {
        return -EINVAL;
    }

    if (self->id->function_table == NULL) {
        return -ENOTSUP;
    }

    ws_regular_command_func func = NULL;
    struct ws_object_function const* iter;
    for (iter = self->id->function_table; func == NULL && iter->name; iter++) {
        if (ws_streq(iter->name, ident)) {
            func = iter->func;
        }
    }

    if (func) {
        return func(stack);
    }

    return -ENOENT;
}

bool
ws_object_has_cmd(
    struct ws_object* self,
    char const* ident
) {
    if (!self || !ident) {
        return false;
    }

    ws_object_lock_read(self);
    ws_object_type_id* curtype = self->id;

    while (curtype != &WS_OBJECT_TYPE_ID_OBJECT) {
        struct ws_object_function const* iter;
        for (iter = self->id->function_table; iter->name; iter++) {
            if (ws_streq(iter->name, ident)) {
                ws_object_unlock(self);
                return true;
            }
        }

        curtype = curtype->supertype;
    }

    ws_object_unlock(self);
    return false;
}

/*
 *
 * static function implementations
 *
 */
