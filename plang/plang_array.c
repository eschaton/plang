/*
    plang_array.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_array_internal.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

PLANG_SOURCE_BEGIN


plang_array_t PLANG_NULLABLE
plang_array_new(const size_t capacity)
{
    assert(capacity > 0);

    plang_array_t array = NULL;

    array = calloc(sizeof(struct plang_array), 1);
    if (array) {
        array->_contents = calloc(sizeof(void *), capacity);
        if (array->_contents == NULL) goto error;
        array->_count = 0;
        array->_capacity = capacity;
    }

    return array;

error:
    plang_array_free(array);
    return NULL;
}


void
plang_array_free(plang_array_t PLANG_NULLABLE array)
{
    if (array == NULL) return;

    free(array->_contents);
    free(array);
}


plang_array_t PLANG_NULLABLE
plang_array_copy(plang_array_t array)
{
    plang_array_t copy = calloc(sizeof(struct plang_array), 1);
    if (copy) {
        /*
         Note that the copy's capacity to the original's count, which
         serves to compact the array contents.
         */
        copy->_count = array->_count;
        copy->_capacity = array->_count;

        copy->_contents = calloc(sizeof(void *), array->_count);
        if (copy->_contents == NULL) goto error;

        for (size_t i = 0; i < array->_count; i++) {
            copy->_contents[i] = array->_contents[i];
        }
    }

    return copy;

error:
    plang_array_free(copy);
    return NULL;
}


plang_array_t PLANG_NULLABLE
plang_array_new_with_items(void *value, ...)
{
    if (value == NULL) return plang_array_new(8);

    size_t count = 1;
    plang_array_t array = NULL;
    void *v = NULL;
    bool appended = false;

    va_list ap, ap2;
    va_start(ap, value);
    va_copy(ap2, ap);
    do {
        v = va_arg(ap, void *);
        if (v) count += 1;
    } while (v != NULL);
    va_end(ap);

    array = plang_array_new(count);
    if (array == NULL) goto error;

    appended = plang_array_append(array, value);
    if (appended == false) goto error;

    va_start(ap2, value);
    for (size_t i = 1; i < count; i++) {
        v = va_arg(ap2, void *);
        appended = plang_array_append(array, v);
        if (appended == false) {
            va_end(ap2);
            goto error;
        }
    }
    va_end(ap2);

    return array;

error:
    plang_array_free(array);
    return NULL;
}


size_t
plang_array_get_count(plang_array_t array)
{
    return array->_count;
}


void *
plang_array_get_item(plang_array_t array,
                     const size_t idx)
{
    assert(idx < array->_count);

    return array->_contents[idx];
}


void
plang_array_set_item(plang_array_t array,
                     const size_t idx,
                     void *value)
{
    assert(idx < array->_count);

    array->_contents[idx] = value;
}


void *
plang_array_get_last_item(plang_array_t array)
{
    assert(array->_count > 0);

    return array->_contents[array->_count - 1];
}


bool
plang_array_expand(plang_array_t array)
{
    if (array->_count < array->_capacity) return true;

    const size_t new_capacity = array->_capacity + 16;
    void * PLANG_NULLABLE * PLANG_NULLABLE new_contents = NULL;

    new_contents = realloc(array->_contents,
                           sizeof(void *) * new_capacity);
    if (new_contents == NULL) return false;

    /* realloc(3) doesn't zero upon expansion */

    for (size_t i = array->_capacity; i < new_capacity; i++) {
        new_contents[i] = NULL;
    }

    array->_contents = new_contents;
    array->_capacity = new_capacity;

    return true;
}


bool
plang_array_append(plang_array_t array,
                   void *value)
{
    if (plang_array_expand(array) == false) return false;

    array->_contents[array->_count] = value;
    array->_count += 1;

    return true;
}


void
plang_array_remove(plang_array_t array,
                   size_t idx)
{
    assert(array->_count < idx);

    const size_t count = array->_count - 1;

    for (size_t i = idx; i < count; i++) {
        array->_contents[i] = array->_contents[i + 1];
    }

    array->_contents[count] = NULL;

    array->_count = count;
}


void
plang_array_remove_last_item(plang_array_t array)
{
    assert(array->_count > 0);

    array->_contents[array->_count - 1] = NULL;

    array->_count -= 1;
}


PLANG_SOURCE_END
