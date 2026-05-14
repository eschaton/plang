/*
    plang_unit.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_unit__h__
#define __plang_unit__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_token *plang_token_t;


/*! A type in `plang`. */
typedef struct plang_unit *plang_unit_t;


plang_unit_t PLANG_NULLABLE
plang_unit_new(plang_parser_t parser,
               plang_token_t identifier,
               plang_node_t node);


void
plang_unit_free(plang_unit_t PLANG_NULLABLE unit);


plang_parser_t
plang_unit_get_parser(plang_unit_t unit);


plang_token_t
plang_unit_get_identifier(plang_unit_t unit);


plang_node_t
plang_unit_get_node(plang_unit_t unit);


PLANG_HEADER_END

#endif /* __plang_unit__h__ */
