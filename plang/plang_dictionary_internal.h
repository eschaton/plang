/*
    plang_dictionary_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_dictionary_internal__h__
#define __plang_dictionary_internal__h__

#include "plang_dictionary.h"

PLANG_HEADER_BEGIN


struct plang_dictionary {
    plang_array_t _keys;
    plang_array_t _values;

    plang_comparator_t _key_comparator;
    void * PLANG_NULLABLE _context;
};


/*!
 The default key comparator.

 Compares keys solely by pointer equality, ignoring the \a context
 parameter entirely.
 */
bool
plang_dictionary_key_comparator_default(void *key1,
                                        void *key2,
                                        void * PLANG_NULLABLE context);


PLANG_HEADER_END

#endif /* __plang_dictionary_internal__h__ */
