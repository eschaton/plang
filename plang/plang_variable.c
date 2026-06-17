/*
    plang_variable.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_variable_internal.h"

#include <assert.h>
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


bool
plang_variable_is_parameter(plang_variable_t variable)
{
    return (plang_node_get_type(variable->_node) ==
            plang_node_type_parameter_declaration);
}


plang_type_t
plang_variable_get_type(plang_variable_t variable,
                        plang_scope_t scope)
{
    /*
     Variables may be introduced by both variable declarations and
     parameter declarations.

     Currently, parameter declarations are only allowed to use type
     identifiers, not full types.
     */

    plang_node_t variable_type = NULL;
    if (plang_variable_is_parameter(variable) == false) {
        variable_type
            = plang_node_variable_declaration_get_type(variable->_node);
    } else {
        variable_type
            = plang_node_parameter_declaration_get_type_identifier(variable->_node);
    }

    plang_type_t type;

    if (plang_node_get_type(variable_type) == plang_node_type_type_identifier) {
        /* Types referenced by an identifier can be looked up by it. */
        plang_token_t type_identifier
            = plang_node_type_identifier_get_identifier(variable_type);
        type = plang_scope_type_lookup(scope, type_identifier, true);
    } else {
        /*
         Types can be implicitly (and anonymously) declared as part of a
         variable declaration, if they have not previously been given an
         identifier via a type declaration.
         */
        type = plang_scope_type_lookup_anonymous_type(scope,
                                                      variable_type,
                                                      true);
    }

    return type;
}


PLANG_SOURCE_END
