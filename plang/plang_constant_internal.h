/*
    plang_constant_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_constant_internal__h__
#define __plang_constant_internal__h__

#include "plang_constant.h"

PLANG_HEADER_BEGIN


struct plang_constant {
    /*! Parser that produced this constant. */
    plang_parser_t _parser;

    /*! Identifier token representing this constant. */
    plang_token_t _identifier;
    
    /*! Node for the constant declaration itself. */
    plang_node_t _node;
};


PLANG_HEADER_END

#endif /* __plang_constant_internal__h__ */
