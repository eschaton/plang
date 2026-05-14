/*
    plang_constant.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_constant_internal.h"

#include <stdlib.h>

#include "plang_parser.h"
#include "plang_token.h"

PLANG_SOURCE_BEGIN


plang_constant_t PLANG_NULLABLE
plang_constant_new(plang_parser_t parser,
                   plang_token_t identifier,
                   plang_node_t node)
{
    struct plang_constant *constant = NULL;

    constant = calloc(sizeof(struct plang_constant), 1);
    if (constant == NULL) return NULL;

    constant->_parser = parser;
    constant->_identifier = identifier;
    constant->_node = node;

    return constant;
}


void
plang_constant_free(plang_constant_t PLANG_NULLABLE constant)
{
    if (constant == NULL) return;

    free(constant);
}


plang_parser_t
plang_constant_get_parser(plang_constant_t constant)
{
    return constant->_parser;
}


plang_token_t
plang_constant_get_identifier(plang_constant_t constant)
{
    return constant->_identifier;
}


plang_node_t
plang_constant_get_node(plang_constant_t constant)
{
    return constant->_node;
}


PLANG_SOURCE_END
