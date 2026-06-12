/*
    plang_source.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_source__h__
#define __plang_source__h__

#include "plang_defines.h"

#include <stdlib.h>

PLANG_HEADER_BEGIN


/*!
 A source of `plang` source text.

 Source text is represented via a buffer, but also has an associated
 _vpath_ (for "virtual path") used to refer to it. When source text is
 supplied from a file, this will be the file's ``realpath(3)``. When it
 is supplied directly, a secure hash of the text (converted to ASCII hex
 digits) will be used.
 */
typedef struct plang_source *plang_source_t;


/*!
 Create a new source of source text.

 If `NULL` is passed for the _vpath_ then a secure hash of the buffer's
 text (converted to ASCII hex digits) is used instead to ensure it can
 be uniquely referenced.
 */
plang_source_t PLANG_NULLABLE
plang_source_new(const char * PLANG_NULLABLE vpath,
                 const char *buf,
                 const size_t buf_len);


/*!
 Create a new source of source text from a file on disk.

 Reads the contents of the file at \a path and returns a new source of
 source text based on it.
 */
plang_source_t PLANG_NULLABLE
plang_source_new_from_file(const char *path);


/*!
 Dispose of a source of source text.
 */
void
plang_source_free(plang_source_t PLANG_NULLABLE source);


/*!
 Get the _vpath_ for the source.
 */
const char *
plang_source_get_vpath(plang_source_t source);


/*!
 Get the length of the source.
 */
size_t
plang_source_get_length(plang_source_t source);


/*!
 Get the character at the given position.
 */
char
plang_source_get_char(plang_source_t source,
                      size_t position);


/*!
 Get the characters starting at the given position.

 - NOTE: This is mostly useful with ranges.
 */
const char * PLANG_NULLABLE
plang_source_get_chars(plang_source_t source,
                       size_t position);


PLANG_HEADER_END

#endif /* __plang_source__h__ */
