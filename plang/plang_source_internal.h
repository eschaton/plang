/*
    plang_source_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_source_internal__h__
#define __plang_source_internal__h__

#include "plang_source.h"

PLANG_HEADER_BEGIN


/*!
 Some `plang` source text.

 In addition to representing source text, this also represents a virtual
 
 */
struct plang_source {
    const char *_vpath;
    const char *_buf;
    size_t _buf_len;
};


/*!
 Create a _vpath_ for the given text data.

 - NOTE: The caller must dispose of the result with ``free(3)``.
 */
const char * PLANG_NULLABLE
plang_source_create_vpath(const char *buf,
                          const size_t buf_len);


PLANG_HEADER_END

#endif /* __plang_source_internal__h__ */
