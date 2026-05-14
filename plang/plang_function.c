/*
    plang_function.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_function_internal.h"

#include <stdlib.h>

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


PLANG_SOURCE_END
