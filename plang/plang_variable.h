/*
    plang_variable.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_variable__h__
#define __plang_variable__h__

#include "plang_defines.h"

#include <stdbool.h>

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_scope *plang_scope_t;
typedef struct plang_token *plang_token_t;
typedef struct plang_type *plang_type_t;


/*! A variable in `plang`. */
typedef struct plang_variable *plang_variable_t;


plang_variable_t PLANG_NULLABLE
plang_variable_new(plang_parser_t parser,
                   plang_token_t identifier,
                   plang_node_t node);


void
plang_variable_free(plang_variable_t PLANG_NULLABLE variable);


plang_parser_t
plang_variable_get_parser(plang_variable_t variable);


plang_token_t
plang_variable_get_identifier(plang_variable_t variable);


plang_node_t
plang_variable_get_node(plang_variable_t variable);


/*! Whether a variable represents a procedure or function parameter. */
bool
plang_variable_is_parameter(plang_variable_t variable);


plang_type_t
plang_variable_get_type(plang_variable_t variable,
                        plang_scope_t scope);


PLANG_HEADER_END

#endif /* __plang_variable__h__ */
