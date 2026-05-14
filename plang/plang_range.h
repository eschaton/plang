/*
    plang_range.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_range__h__
#define __plang_range__h__

#include "plang_defines.h"

#include <unistd.h>

PLANG_HEADER_BEGIN


/*!
 A range of text.
 */
struct plang_range {
    size_t start;
    size_t length;
};
typedef struct plang_range plang_range_t;


/*!
 Create a range.
 */
__attribute__((always_inline))
static inline
plang_range_t
plang_range(const size_t start,
            const size_t length)
{
    plang_range_t range;

    range.start = start;
    range.length = length;

    return range;
}


PLANG_HEADER_END

#endif /* __plang_range__h__ */
