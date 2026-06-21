/*
    plang_type_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_type_internal__h__
#define __plang_type_internal__h__

#include "plang_type.h"

PLANG_HEADER_BEGIN


struct plang_type {
    plang_parser_t _parser;
    plang_token_t _identifier;
    plang_node_t _node;
    plang_type_kind_t _kind;
    plang_node_t PLANG_NULLABLE _concrete_node;
};


PLANG_HEADER_END

#endif /* __plang_type_internal__h__ */
