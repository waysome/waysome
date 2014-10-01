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

#ifndef __WS_OBJECTS_OBJECT_H__
#define __WS_OBJECTS_OBJECT_H__

#include <stdbool.h>
#include <pthread.h>

/*
 * Type names
 */
struct ws_object_type;
struct ws_object;

/**
 * object type identification
 *
 * References to a single ws_object_type. Used for indicating an object type by
 * a single pointer.
 */
typedef struct ws_object_type const ws_object_type_id;

/*
 *
 * Object callback function types
 *
 */

/**
 * Constructor callback
 */
typedef bool (*ws_object_init_callback)(struct ws_object* const);

/**
 * Destructor callback
 */
typedef bool (*ws_object_deinit_callback)(struct ws_object* const);

/**
 * log callback
 *
 * @todo Implement second parameter: struct ws_log_context*
 */
typedef bool (*ws_object_log_callback)(struct ws_object* const, void*);

/**
 * run callback
 */
typedef bool (*ws_object_run_callback)(struct ws_object* const);

/*
 *
 * Type implementation
 *
 */

/**
 * Object type identifier for identifiying an object type
 */
struct ws_object_type {
    ws_object_type_id* supertype;     //!< supertype. Ref to itself if there is none
    const char* const typestr;  //!< string which represents the type

    ws_object_init_callback init_callback; //!< Init callback for the type
    ws_object_deinit_callback deinit_callback; //!< Free callback for the type
    ws_object_log_callback log_callback; //!< Log callback for the type
    ws_object_run_callback run_callback; //!< Run callback for the type
};

/**
 * Object settings type
 *
 * For identifying an object "setting"
 */
enum ws_object_settings {
    WS_OBJ_NO_SETTINGS = 0,
    WS_OBJ_CONST            = 1 << 0,
    WS_OBJ_SELF_DESTROYING  = 1 << 1,
    WS_OBJECT_STACKALLOCED  = 1 << 2,
    WS_OBJECT_LOCKABLE      = 1 << 3,
};

/**
 * Object type
 *
 * @todo implement locking and refcounting
 *
 * The object type is the root class for all classes
 */
struct ws_object {
    ws_object_type_id* id;        //!< Object id, identifies the actual type

    struct {
        pthread_rwlock_t rwl;
        size_t refcnt;
    } ref_counting; //!< Ref counting

    enum ws_object_settings settings; //!< Object settings
    pthread_rwlock_t rw_lock; //!< Read/Write lock
};

/**
 * Variable which holds type information about the ws_object type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_OBJECT;

/**
 * Allocate an object with a given size
 *
 * @memberof ws_object
 *
 * @note One ref on the object is used
 *
 * @return A new ws_object object or NULL on failure
 */
struct ws_object*
ws_object_new(
    size_t
);

/**
 * Allocate a new plain ws_object object.
 *
 * @memberof ws_object
 *
 * @note One ref on the object is used
 *
 * "plain" means that the size of the object is `sizeof(struct ws_object)`
 *
 * @return A new ws_object object or NULL on failure
 */
struct ws_object*
ws_object_new_raw(void);

/**
 * Get the ID of the object
 *
 * @memberof ws_object
 *
 * @return The id of the object or NULL on failure
 */
ws_object_type_id*
ws_object_get_type_id(
    struct ws_object* const self //!< The object
);

/**
 * Get the settings of an object
 *
 * @memberof ws_object
 *
 * @return The settings
 */
enum ws_object_settings
ws_object_get_settings(
    struct ws_object* const self //!< The object
);

/**
 * Set the settings of an object
 *
 * @memberof ws_object
 *
 * @note This should only be done _once_.
 */
void
ws_object_set_settings(
    struct ws_object* self, //!< The object
    enum ws_object_settings //!< The settings to set
);

/**
 * Initialize an object
 *
 * @memberof ws_object
 *
 * @note One ref of the object is used
 *
 * @return true if the object was initialized successfully, else false
 */
bool
ws_object_init(
    struct ws_object* self //!< The object
);

/**
 * Get ref on the object
 *
 * @memberof ws_object
 *
 * @return The object itself or NULL on failure
 */
struct ws_object*
ws_object_getref(
    struct ws_object* self //!< The object
);

/**
 * Unreference an object
 *
 * @memberof ws_object
 *
 * @warning It is not save to use the object after this operation _in any kind_.
 * The object might be unavailable after this operation, as it was freed from
 * the heap.
 */
void
ws_object_unref(
    struct ws_object* self //!< The object
);

/**
 * Log the object with the passed context
 *
 * @memberof ws_object
 *
 * @return true if the logging was successful, else false
 */
bool
ws_object_log(
    struct ws_object const* const self, //!< The object
    void* log_context /*!< @todo ws_log_context* */
);

/**
 * Run the object
 *
 * @memberof ws_object
 *
 * @note returns false also if there is no run callback available
 *
 * @return true if the object is run, else false
 */
bool
ws_object_run(
    struct ws_object* self //!< The object
);

/**
 * Read-lock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was aquired, false on error
 */
bool
ws_object_lock_read(
    struct ws_object* self //!< The object
);

/**
 * Write-lock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was aquired, false on error
 */
bool
ws_object_lock_write(
    struct ws_object* self //!< The object
);

/**
 * Read-unlock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was unlocked, else false
 */
bool
ws_object_unlock_read(
    struct ws_object* self //!< The object
);

/**
 * Write-unlock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was unlocked, else false
 */
bool
ws_object_unlock_write(
    struct ws_object* self //!< The object
);

#endif // __WS_OBJECTS_OBJECT_H__

/**
 * @}
 */
