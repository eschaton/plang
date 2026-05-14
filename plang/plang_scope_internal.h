/*
    plang_scope_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_scope_internal__h__
#define __plang_scope_internal__h__

#include "plang_scope.h"

PLANG_HEADER_BEGIN


/* Forward declarations */
typedef struct plang_dictionary *plang_dictionary_t;


/*!
 Underlying structure of a lexical scope.

 Lexcical scopes are nested; the outermost scope has no parent.
 */
struct plang_scope {
    /*! The parent scope of this scope. */
    struct plang_scope * PLANG_NULLABLE _parent;

    plang_dictionary_t _constants;
    plang_dictionary_t _types;
    plang_dictionary_t _variables;
    plang_dictionary_t _procedures;
    plang_dictionary_t _functions;
};


bool
plang_scope_item_register(plang_dictionary_t dictionary,
                          plang_token_t identifier,
                          void *item);


void * PLANG_NULLABLE
plang_scope_item_lookup(plang_dictionary_t dictionary,
                        plang_token_t identifier);


PLANG_HEADER_END

#endif /* __plang_scope_internal__h__ */
