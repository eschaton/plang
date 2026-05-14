/*
    plang_array.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_array__h__
#define __plang_array__h__

#include "plang_defines.h"

#include <stdbool.h>
#include <unistd.h>

PLANG_HEADER_BEGIN


/*! A pointer array of variable capacity. */
typedef struct plang_array *plang_array_t;


plang_array_t PLANG_NULLABLE
plang_array_new(const size_t capacity);

void
plang_array_free(plang_array_t PLANG_NULLABLE array);

/*!
 Returns a copy of the array.

 - WARNING: This copies the array only, not the contents.
 */
plang_array_t PLANG_NULLABLE
plang_array_copy(plang_array_t array);

plang_array_t PLANG_NULLABLE
plang_array_new_with_items(void *value, ...);

size_t
plang_array_get_count(plang_array_t array);

void *
plang_array_get_item(plang_array_t array,
                     const size_t idx);

void
plang_array_set_item(plang_array_t array,
                     size_t idx,
                     void *value);

void *
plang_array_get_last_item(plang_array_t array);

bool
plang_array_append(plang_array_t array,
                   void *value);

void
plang_array_remove(plang_array_t array,
                   size_t idx);

void
plang_array_remove_last_item(plang_array_t array);


PLANG_HEADER_END

#endif /* __plang_array__h__ */
