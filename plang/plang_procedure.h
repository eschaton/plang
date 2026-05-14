/*
    plang_procedure.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_procedure__h__
#define __plang_procedure__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_token *plang_token_t;


/*! A procedure in `plang`. */
typedef struct plang_procedure *plang_procedure_t;


plang_procedure_t PLANG_NULLABLE
plang_procedure_new(plang_parser_t parser,
                    plang_token_t identifier,
                    plang_node_t node);


void
plang_procedure_free(plang_procedure_t PLANG_NULLABLE procedure);


plang_parser_t
plang_procedure_get_parser(plang_procedure_t procedure);


plang_token_t
plang_procedure_get_identifier(plang_procedure_t procedure);


plang_node_t
plang_procedure_get_node(plang_procedure_t procedure);


PLANG_HEADER_END

#endif /* __plang_procedure__h__ */
