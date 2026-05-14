/*
    plang_program.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_program__h__
#define __plang_program__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_token *plang_token_t;


/*! A type in `plang`. */
typedef struct plang_program *plang_program_t;


plang_program_t PLANG_NULLABLE
plang_program_new(plang_parser_t parser,
                  plang_token_t identifier,
                  plang_node_t node);


void
plang_program_free(plang_program_t PLANG_NULLABLE program);


plang_parser_t
plang_program_get_parser(plang_program_t program);


plang_token_t
plang_program_get_identifier(plang_program_t program);


plang_node_t
plang_program_get_node(plang_program_t program);


PLANG_HEADER_END

#endif /* __plang_program__h__ */
