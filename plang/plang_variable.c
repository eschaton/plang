/*
    plang_variable.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_variable_internal.h"

#include <stdlib.h>

#include "plang_node.h"
#include "plang_scope.h"
#include "plang_token.h"
#include "plang_type.h"

PLANG_SOURCE_BEGIN


plang_variable_t PLANG_NULLABLE
plang_variable_new(plang_parser_t parser,
                   plang_token_t identifier,
                   plang_node_t node)
{
    struct plang_variable *variable = NULL;

    variable = calloc(sizeof(struct plang_variable), 1);
    if (variable == NULL) return NULL;

    variable->_parser = parser;
    variable->_identifier = identifier;
    variable->_node = node;

    return variable;
}


void
plang_variable_free(plang_variable_t PLANG_NULLABLE variable)
{
    if (variable == NULL) return;

    free(variable);
}


plang_parser_t
plang_variable_get_parser(plang_variable_t variable)
{
    return variable->_parser;
}


plang_token_t
plang_variable_get_identifier(plang_variable_t variable)
{
    return variable->_identifier;
}


plang_node_t
plang_variable_get_node(plang_variable_t variable)
{
    return variable->_node;
}


plang_type_t
plang_variable_get_type(plang_variable_t variable,
                        plang_scope_t scope)
{
    plang_node_t variable_type
        = plang_node_variable_declaration_get_type(variable->_node);
    plang_node_t type_declaration
        = plang_node_type_get_type_declaration(variable_type);
    plang_token_t type_identifier
        = plang_node_type_declaration_get_identifier(type_declaration);
    plang_type_t type = plang_scope_type_lookup(scope,
                                                type_identifier,
                                                true);
    return type;
}


PLANG_SOURCE_END
