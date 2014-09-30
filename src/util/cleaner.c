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
#include <stdlib.h>

#include "util/cleaner.h"

/**
 * Cleaner linked list entry
 */
struct cleaner_entry {
    struct cleaner_entry* next; //!< Next cleaner entry
    void (*func)(void*); //!< Function to call
    void* etc; //!< Argument for the function
};

/**
 * Cleaner singleton
 *
 * Insertion should be at beginning, calling the linked list from the first
 * entry to the last. So the first inserted entry can be the cleaner for the
 * cleaner itself.
 */
static struct cleaner {
    struct cleaner_entry* head; //!< Head of linked list for clean functions
    pthread_mutex_t mtx; //!< Mutex for cleanup linked list
} cleaner;


/*
 *
 * Interface implementation
 *
 */

void
ws_cleaner_init(void)
{
    cleaner.head = NULL;
    pthread_mutex_init(&cleaner.mtx, NULL);
}

int
ws_cleaner_add(
    void (*cleaner_func)(void*),
    void* etc
) {
    struct cleaner_entry* e = calloc(1, sizeof(*e));

    if (!e) {
        return -ENOMEM;
    }

    e->etc = etc;
    e->func = cleaner_func;

    pthread_mutex_lock(&cleaner.mtx);
    e->next = cleaner.head;
    cleaner.head = e;
    pthread_mutex_unlock(&cleaner.mtx);

    return 0;
}

bool
ws_cleaner_run(void)
{
    pthread_mutex_lock(&cleaner.mtx);
    struct cleaner_entry* iter = cleaner.head;
    struct cleaner_entry* next;

    while (iter) {
        next = iter->next;
        iter->func(iter->etc);
        free(iter);
        iter = next;
    }

    pthread_mutex_destroy(&cleaner.mtx);
    return true;
}

