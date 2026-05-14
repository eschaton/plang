/*
    plang_variable_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_variable_internal__h__
#define __plang_variable_internal__h__

#include "plang_variable.h"

PLANG_HEADER_BEGIN


struct plang_variable {
    plang_parser_t _parser;
    plang_token_t _identifier;
    plang_node_t _node;
};


PLANG_HEADER_END

#endif /* __plang_variable_internal__h__ */
