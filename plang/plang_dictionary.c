/*
    plang_dictionary.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_dictionary_internal.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "plang_array.h"

PLANG_SOURCE_BEGIN


#define PLANG_NOT_FOUND SIZE_T_MAX


plang_dictionary_t PLANG_NULLABLE
plang_dictionary_new(size_t capacity)
{
    assert(capacity > 0);
    
    struct plang_dictionary *dict = NULL;
    const size_t real_capacity = (capacity > 0) ? capacity : 8;

    dict = calloc(sizeof(struct plang_dictionary), 1);
    if (dict == NULL) goto error;

    dict->_keys = plang_array_new(real_capacity);
    if (dict->_keys == NULL) goto error;

    dict->_values = plang_array_new(real_capacity);
    if (dict->_values == NULL) goto error;

    dict->_key_comparator = plang_dictionary_key_comparator_default;
    dict->_context = NULL;

    return dict;

error:
    plang_dictionary_free(dict);
    return NULL;
}


void
plang_dictionary_free(plang_dictionary_t PLANG_NULLABLE dict)
{
    if (dict == NULL) return;

    plang_array_free(dict->_keys);
    plang_array_free(dict->_values);

    free(dict);
}


plang_dictionary_t PLANG_NULLABLE
plang_dictionary_copy(plang_dictionary_t dict)
{
    struct plang_dictionary *copy
        = calloc(sizeof(struct plang_dictionary), 1);
    if (copy) {
        copy->_keys = plang_array_copy(dict->_keys);
        if (copy->_keys == NULL) goto error;

        copy->_values = plang_array_copy(dict->_values);
        if (copy->_values == NULL) goto error;

        copy->_key_comparator = dict->_key_comparator;
        copy->_context = dict->_context;
    }

    return copy;

error:
    plang_dictionary_free(copy);
    return NULL;
}


size_t
plang_dictionary_get_count(plang_dictionary_t dict)
{
    return plang_array_get_count(dict->_keys);
}


static inline
size_t
plang_dictionary_get_key_index(plang_dictionary_t dict,
                               void *key)
{
    plang_array_t keys = dict->_keys;
    plang_comparator_t comparator = dict->_key_comparator;
    void *context = dict->_context;

    const size_t count = plang_array_get_count(dict->_keys);

    for (size_t i = 0; i < count; i++) {
        void *potential = plang_array_get_item(keys, i);
        if (comparator(key, potential, context)) {
            return i;
        }
    }

    return PLANG_NOT_FOUND;
}


bool
plang_dictionary_key_comparator_default(void *key1,
                                        void *key2,
                                        void * PLANG_NULLABLE context)
{
    return (key1 == key2);
}


void * PLANG_NULLABLE
plang_dictionary_get(plang_dictionary_t dict,
                     void *key)
{
    size_t idx = plang_dictionary_get_key_index(dict, key);
    if (idx != PLANG_NOT_FOUND) {
        return plang_array_get_item(dict->_values, idx);
    }

    return NULL;
}


bool
plang_dictionary_set(plang_dictionary_t dict,
                     void *key,
                     void *value)
{
    size_t idx = plang_dictionary_get_key_index(dict, key);
    if (idx != PLANG_NOT_FOUND) {
        plang_array_set_item(dict->_keys, idx, key);
        plang_array_set_item(dict->_values, idx, value);
        return true;
    } else {
        bool appended_key = plang_array_append(dict->_keys, key);
        bool appended_value = plang_array_append(dict->_values, value);
        return (appended_key && appended_value);
    }
}


void
plang_dictionary_remove(plang_dictionary_t dict,
                        void *key)
{
    size_t idx = plang_dictionary_get_key_index(dict, key);
    assert(idx != PLANG_NOT_FOUND);

    plang_array_remove(dict->_keys, idx);
    plang_array_remove(dict->_values, idx);
}


plang_array_t
plang_dictionary_copy_all_keys(plang_dictionary_t dict)
{
    return plang_array_copy(dict->_keys);
}


plang_array_t
plang_dictionary_copy_all_values(plang_dictionary_t dict)
{
    return plang_array_copy(dict->_values);
}


void
plang_dictionary_set_key_comparator(plang_dictionary_t dict,
                                    plang_comparator_t PLANG_NULLABLE comp,
                                    void * PLANG_NULLABLE context)
{
    plang_comparator_t comparator
        = ((comp != NULL)
           ? comp
           : plang_dictionary_key_comparator_default);
    dict->_key_comparator = comparator;
    dict->_context = context;
}


PLANG_SOURCE_END
