/*
    plang_unit.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_unit_internal.h"

#include <stdlib.h>

#include "plang_node.h"


PLANG_SOURCE_BEGIN


plang_unit_t PLANG_NULLABLE
plang_unit_new(plang_parser_t parser,
               plang_token_t identifier,
               plang_node_t node)
{
    struct plang_unit *unit = NULL;

    unit = calloc(sizeof(struct plang_unit), 1);
    if (unit == NULL) return NULL;

    unit->_parser = parser;
    unit->_identifier = identifier;
    unit->_node = node;

    return unit;
}


void
plang_unit_free(plang_unit_t PLANG_NULLABLE unit)
{
    if (unit == NULL) return;

    free(unit);
}


plang_parser_t
plang_unit_get_parser(plang_unit_t unit)
{
    return unit->_parser;
}


plang_token_t
plang_unit_get_identifier(plang_unit_t unit)
{
    return unit->_identifier;
}


plang_node_t
plang_unit_get_node(plang_unit_t unit)
{
    return unit->_node;
}


plang_scope_t
plang_unit_get_interface_scope(plang_unit_t unit)
{
    return plang_node_unit_get_interface_scope(unit->_node);
}


plang_scope_t
plang_unit_get_implementation_scope(plang_unit_t unit)
{
    return plang_node_unit_get_implementation_scope(unit->_node);
}


#if PLANG_CLASCAL
plang_array_t PLANG_NULLABLE
plang_unit_copy_method_blocks(plang_unit_t unit)
{
    return plang_node_unit_copy_method_blocks(unit->_node);
}
#endif


PLANG_SOURCE_END
