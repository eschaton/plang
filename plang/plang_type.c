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
#include "plang_token.h"

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

    /*
     If the type's node is a type identifier, then the type itself
     is a built-in type and already concrete. Otherwise it needs to
     be looked up further until it reaches a concrete type.
     */

    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return tdnode;
    }

    /*
     The type's node must be a type declaration. Look up the type on the
     right-hand side of the type declaration until a concrete type is
     discovered rather than a type identifier.
     */

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

            /*
             If the type declaration's type is self-referential, that
             that means it's a built-in type.
             */

            if (tdnode == tdtype) {
                ctnode = tdnode;
            }
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

    /*
     If the type's node is a type identifier, that means it's a built-in
     type. Whether it's an ordinal depends on which built-in type it is.
     */

    plang_node_t tdnode = type->_node;
    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return plang_token_identifier_is_built_in_ordinal_type(type->_identifier);
    }

    /*
     Otherwise, recursively determine the concrete type to which this
     type corresponds, and return whether that's an ordinal type.
     */

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents an ordinal type or whether it was a red herring.
     */

    if (ctnode == NULL) return false;

    if (ctnode_type == plang_node_type_type_identifier) {
        plang_token_t ctnode_identifier
            = plang_node_type_identifier_get_identifier(ctnode);
        return plang_token_identifier_is_built_in_ordinal_type(ctnode_identifier);
    }

    return ((ctnode_type == plang_node_type_subrange_type) ||
            (ctnode_type == plang_node_type_enumerated_type));
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

    /*
     If the type's node is a type identifier, that means it's a built-in
     type. Since there's no such thing as a built-in string type, this
     can't be one.
     */

    plang_node_t tdnode = type->_node;
    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return false;
    }

    /*
     Otherwise, recursively determine the concrete type to which this
     type corresponds, and return whether that's a string type.
     */

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

    /*
     If the type's node is a type identifier, that means it's a built-in
     type. Since there's no such thing as a built-in structured type,
     this can't be one.
     */

    plang_node_t tdnode = type->_node;
    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return false;
    }

    /*
     Otherwise, recursively determine the concrete type to which this
     type corresponds, and return whether that's a structured type.
     */

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

    /*
     If the type's node is a type identifier, that means it's a built-in
     type. Since there's no such thing as a built-in record type, this
     can't be one.
     */

    plang_node_t tdnode = type->_node;
    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return false;
    }

    /*
     Otherwise, recursively determine the concrete type to which this
     type corresponds, and return whether that's a record type.
     */

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

    /*
     If the type's node is a type identifier, that means it's a built-in
     type. Whether it's a pointer depends on which built-in type it is.
     */

    plang_node_t tdnode = type->_node;
    plang_node_type_t tdnode_type = plang_node_get_type(tdnode);
    if (tdnode_type == plang_node_type_type_identifier) {
        return plang_token_identifier_is_built_in_pointer_type(type->_identifier);
    }

    /*
     Otherwise, recursively determine the concrete type to which this
     type corresponds, and return whether that's a pointer type.
     */

    plang_node_t ctnode = plang_type_get_concrete_type_node(type,
                                                            scope);
    plang_node_type_t ctnode_type
        = (ctnode != NULL) ? plang_node_get_type(ctnode)
                           : plang_node_type_unknown;

    /*
     Return whether the concrete type node (if any was found)
     represents a pointer type or whether it was a red herring.
     */

    if (ctnode == NULL) return false;

    if (ctnode_type == plang_node_type_type_identifier) {
        plang_token_t ctnode_identifier
            = plang_node_type_identifier_get_identifier(ctnode);
        return plang_token_identifier_is_built_in_pointer_type(ctnode_identifier);
    }

    return (ctnode_type == plang_node_type_pointer_type);
}


PLANG_SOURCE_END
