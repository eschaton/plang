/*
    plang_type.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_type_internal.h"

#include <assert.h>
#include <stdlib.h>

#include "plang_node.h"
#include "plang_scope.h"

PLANG_SOURCE_BEGIN


plang_type_t PLANG_NULLABLE
plang_type_new(plang_parser_t parser,
               plang_token_t identifier,
               plang_node_t node)
{
    struct plang_type *type = NULL;

    type = calloc(sizeof(struct plang_type), 1);
    if (type == NULL) return NULL;

    type->_parser = parser;
    type->_identifier = identifier;
    type->_node = node;

    return type;
}


void
plang_type_free(plang_type_t PLANG_NULLABLE type)
{
    if (type == NULL) return;

    free(type);
}


plang_parser_t
plang_type_get_parser(plang_type_t type)
{
    return type->_parser;
}


plang_token_t
plang_type_get_identifier(plang_type_t type)
{
    return type->_identifier;
}


plang_node_t
plang_type_get_node(plang_type_t type)
{
    return type->_node;
}


plang_node_t PLANG_NULLABLE
plang_type_get_concrete_type_node(plang_type_t PLANG_NULLABLE type,
                                  plang_scope_t scope)
{
    plang_node_t ctnode = NULL;

    plang_node_t tdnode = type->_node;
    do {
        plang_node_t tdtype
            = plang_node_type_declaration_get_type(tdnode);
        assert(tdtype != NULL);
        plang_node_type_t tdtype_type = plang_node_get_type(tdtype);
        if (tdtype_type == plang_node_type_type_identifier) {
            plang_token_t tdtype_identifier
                = plang_node_type_identifier_get_identifier(tdtype);
            plang_type_t tdtype_underlying
                = plang_scope_type_lookup(scope,
                                          tdtype_identifier,
                                          true);
            tdnode = tdtype_underlying->_node;
        } else {
            ctnode = tdtype;
        }
    } while ((tdnode != NULL) && (ctnode == NULL));

    return ctnode;
}


bool
plang_type_is_ordinal(plang_type_t PLANG_NULLABLE type,
                      plang_scope_t scope)
{
    if (type == NULL) return false;

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents an ordinal type or whether it was a red herring.
     */

    return ((ctnode != NULL) &&
            ((ctnode_type == plang_node_type_subrange_type) ||
             (ctnode_type == plang_node_type_enumerated_type)));
}


bool
plang_type_is_real(plang_type_t PLANG_NULLABLE type,
                   plang_scope_t scope)
{
    if (type == NULL) return false;

    // TODO: Support "real" (floating-point) types

    return false;
}


bool
plang_type_is_string(plang_type_t PLANG_NULLABLE type,
                     plang_scope_t scope)
{
    if (type == NULL) return false;

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents a string type or whether it was a red herring.
     */

    return ((ctnode != NULL) &&
            (ctnode_type == plang_node_type_string_type));
}


bool
plang_type_is_structured(plang_type_t PLANG_NULLABLE type,
                         plang_scope_t scope)
{
    if (type == NULL) return false;

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents a structured type or whether it was a red herring.
     */

    return ((ctnode != NULL) &&
            ((ctnode_type == plang_node_type_array_type) ||
             (ctnode_type == plang_node_type_set_type) ||
             (ctnode_type == plang_node_type_record_type)));
}


bool
plang_type_is_record(plang_type_t PLANG_NULLABLE type,
                     plang_scope_t scope)
{
    if (type == NULL) return false;

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents a record type or whether it was a red herring.
     */

    return ((ctnode != NULL) &&
            (ctnode_type == plang_node_type_record_type));
}


bool
plang_type_is_pointer(plang_type_t PLANG_NULLABLE type,
                      plang_scope_t scope)
{
    if (type == NULL) return false;

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents a pointer type or whether it was a red herring.
     */

    return ((ctnode != NULL) &&
            (ctnode_type == plang_node_type_pointer_type));
}


PLANG_SOURCE_END
