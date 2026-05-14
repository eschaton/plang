/*
    plang_dictionary.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_dictionary__h__
#define __plang_dictionary__h__

#include "plang_defines.h"

#include <stdbool.h>
#include <unistd.h>

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_array *plang_array_t;


/*! A dictionary of non-`NULL` key-value pairs. */
typedef struct plang_dictionary *plang_dictionary_t;


plang_dictionary_t PLANG_NULLABLE
plang_dictionary_new(size_t capacity);


void
plang_dictionary_free(plang_dictionary_t PLANG_NULLABLE dict);


/*!
 Returns a copy of the dictionary.

 - WARNING: This copies the dictionary only, not the contents.
 */
plang_dictionary_t PLANG_NULLABLE
plang_dictionary_copy(plang_dictionary_t dict);


size_t
plang_dictionary_get_count(plang_dictionary_t dict);


void * PLANG_NULLABLE
plang_dictionary_get(plang_dictionary_t dict,
                     void *key);


bool
plang_dictionary_set(plang_dictionary_t dict,
                     void *key,
                     void *value);


void
plang_dictionary_remove(plang_dictionary_t dict,
                        void *key);


plang_array_t PLANG_NULLABLE
plang_dictionary_copy_all_keys(plang_dictionary_t dict);


plang_array_t PLANG_NULLABLE
plang_dictionary_copy_all_values(plang_dictionary_t dict);


/*!
 A function that compares two keys for equality.

 Returns `true` if the keys are equal, passed \a context to assist in
 the assessment if necessary.
 */
typedef bool (*plang_comparator_t)(void *k1,
                                   void *k2,
                                   void * PLANG_NULLABLE context);


/*!
 Set the key comparator.

 Setting the key comparator to `NULL` sets it to the default comparator,
 which compares keys solely by pointer and does not use the context.

 The \a context value is passed to \a comp whenever it's invoked.
 */
void
plang_dictionary_set_key_comparator(plang_dictionary_t dict,
                                    plang_comparator_t PLANG_NULLABLE comp,
                                    void * PLANG_NULLABLE context);


PLANG_HEADER_END

#endif /* __plang_dictionary__h__ */
