/*
    plang_procedure.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_procedure_internal.h"

#include <stdlib.h>

PLANG_SOURCE_BEGIN


plang_procedure_t PLANG_NULLABLE
plang_procedure_new(plang_parser_t parser,
                    plang_token_t identifier,
                    plang_node_t node)
{
    struct plang_procedure *procedure = NULL;

    procedure = calloc(sizeof(struct plang_procedure), 1);
    if (procedure == NULL) return NULL;

    procedure->_parser = parser;
    procedure->_identifier = identifier;
    procedure->_node = node;

    return procedure;
}


void
plang_procedure_free(plang_procedure_t PLANG_NULLABLE procedure)
{
    if (procedure == NULL) return;

    free(procedure);
}


plang_parser_t
plang_procedure_get_parser(plang_procedure_t procedure)
{
    return procedure->_parser;
}


plang_token_t
plang_procedure_get_identifier(plang_procedure_t procedure)
{
    return procedure->_identifier;
}


plang_node_t
plang_procedure_get_node(plang_procedure_t procedure)
{
    return procedure->_node;
}


PLANG_SOURCE_END
