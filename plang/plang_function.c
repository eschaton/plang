/*
    plang_function.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_function_internal.h"

#include <stdlib.h>

#include "plang_node.h"
#include "plang_scope.h"
#include "plang_type.h"

PLANG_SOURCE_BEGIN


plang_function_t PLANG_NULLABLE
plang_function_new(plang_parser_t parser,
                   plang_token_t identifier,
                   plang_node_t node)
{
    struct plang_function *function = NULL;
    
    function = calloc(sizeof(struct plang_function), 1);
    if (function == NULL) return NULL;
    
    function->_parser = parser;
    function->_identifier = identifier;
    function->_node = node;
    
    return function;
}


void
plang_function_free(plang_function_t PLANG_NULLABLE function)
{
    if (function == NULL) return;
    
    free(function);
}


plang_parser_t
plang_function_get_parser(plang_function_t function)
{
    return function->_parser;
}


plang_token_t
plang_function_get_identifier(plang_function_t function)
{
    return function->_identifier;
}


plang_node_t
plang_function_get_node(plang_function_t function)
{
    return function->_node;
}


plang_type_t
plang_function_get_result_type(plang_function_t function,
                               plang_scope_t scope)
{
    plang_node_t heading_node = function->_node;
    plang_node_t result_type_node
        = plang_node_function_heading_get_result_type(heading_node);
    plang_node_t type_node
        = plang_node_result_type_get_type(result_type_node);

    plang_type_t type = NULL;
    if (plang_node_get_type(type_node) == plang_node_type_type_identifier) {
        /* For an identified type, get the identifier and resolve it. */

        plang_token_t identifier
            = plang_node_type_identifier_get_identifier(type_node);

        type = plang_scope_type_lookup(scope, identifier, true);
    } else {
        /* For an anonymous type, resolve it directly. */

        type = plang_scope_type_lookup_anonymous_type(scope,
                                                      type_node,
                                                      true);
    }

    return type;
}


PLANG_SOURCE_END
