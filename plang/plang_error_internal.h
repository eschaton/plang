/*
    plang_error_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_error_internal__h__
#define __plang_error_internal__h__

#include "plang_error.h"

PLANG_HEADER_BEGIN


/*!
 A parse error.
 */
struct plang_error {
    plang_error_type_t _type;           /*!< Type of error. */
    plang_source_t _source;             /*!< File containing error. */
    plang_range_t _range;               /*!< Textual range of error. */
};


/*!
 Get the message for the given error type.

 - WARNING: The return value is a string literal neither of whose
            contents nor pointer can change.
 */
const char * const
plang_error_message_for_type(plang_error_type_t type);


PLANG_HEADER_END

#endif /* __plang_error_internal__h__ */
