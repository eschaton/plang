/*
    plang_array_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_array_internal__h__
#define __plang_array_internal__h__

#include "plang_array.h"

PLANG_HEADER_BEGIN


struct plang_array {
    void * PLANG_NULLABLE * PLANG_NULLABLE _contents;
    size_t _count;
    size_t _capacity;
};


bool
plang_array_expand(plang_array_t array);


PLANG_HEADER_END

#endif /* plang_array_internal__h__ */
