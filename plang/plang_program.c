/*
    plang_program.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_program_internal.h"

#include <stdlib.h>

PLANG_SOURCE_BEGIN


plang_program_t PLANG_NULLABLE
plang_program_new(plang_parser_t parser,
                  plang_token_t identifier,
                  plang_node_t node)
{
    struct plang_program *program = NULL;

    program = calloc(sizeof(struct plang_program), 1);
    if (program == NULL) return NULL;

    program->_parser = parser;
    program->_identifier = identifier;
    program->_node = node;

    return program;
}


void
plang_program_free(plang_program_t PLANG_NULLABLE program)
{
    if (program == NULL) return;

    free(program);
}


plang_parser_t
plang_program_get_parser(plang_program_t program)
{
    return program->_parser;
}


plang_token_t
plang_program_get_identifier(plang_program_t program)
{
    return program->_identifier;
}


plang_node_t
plang_program_get_node(plang_program_t program)
{
    return program->_node;
}


PLANG_SOURCE_END
