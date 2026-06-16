/*
    plang_function.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_function__h__
#define __plang_function__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_scope *plang_scope_t;
typedef struct plang_token *plang_token_t;
typedef struct plang_type *plang_type_t;


/*! A function in `plang`. */
typedef struct plang_function *plang_function_t;


plang_function_t PLANG_NULLABLE
plang_function_new(plang_parser_t parser,
                    plang_token_t identifier,
                    plang_node_t node);


void
plang_function_free(plang_function_t PLANG_NULLABLE function);


plang_parser_t
plang_function_get_parser(plang_function_t function);


plang_token_t
plang_function_get_identifier(plang_function_t function);


plang_node_t
plang_function_get_node(plang_function_t function);


plang_type_t
plang_function_get_result_type(plang_function_t function,
                               plang_scope_t scope);


PLANG_HEADER_END

#endif /* __plang_function__h__ */
