/*
    plang_constant.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_constant__h__
#define __plang_constant__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_token *plang_token_t;


/*! A constant in `plang`. */
typedef struct plang_constant *plang_constant_t;


plang_constant_t PLANG_NULLABLE
plang_constant_new(plang_parser_t parser,
                   plang_token_t identifier,
                   plang_node_t node);


void
plang_constant_free(plang_constant_t PLANG_NULLABLE constant);


plang_parser_t
plang_constant_get_parser(plang_constant_t constant);


plang_token_t
plang_constant_get_identifier(plang_constant_t constant);


plang_node_t
plang_constant_get_node(plang_constant_t constant);


PLANG_HEADER_END

#endif /* __plang_constant__h__ */
