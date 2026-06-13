/*
    plang_token_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_token_internal__h__
#define __plang_token_internal__h__

#include "plang_token.h"

PLANG_HEADER_BEGIN


/*!
 The internals of a `plang` token.
 */
struct plang_token {
    plang_token_type_t _type;   /*!< Token type. */
    plang_source_t _source;     /*!< Source contining token. */
    plang_range_t _range;       /*!< Textual range of token. */
};


PLANG_HEADER_END

#endif /* __plang_token_internal__h__ */
