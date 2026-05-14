/*
    plang_parser.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_parser_internal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "plang_array.h"
#include "plang_dictionary.h"
#include "plang_error.h"
#include "plang_node.h"
#include "plang_program.h"
#include "plang_scope.h"
#include "plang_source.h"
#include "plang_token.h"
#include "plang_unit.h"

PLANG_SOURCE_BEGIN


plang_parser_t PLANG_NULLABLE
plang_parser_new(plang_array_t sources)
{
    assert(plang_array_get_count(sources) > 0);

    plang_parser_t parser = calloc(sizeof(struct plang_parser), 1);
    if (parser) {
        parser->_preamble_source = NULL;
        parser->_preamble_unit = NULL;

        parser->_sources = sources;

        plang_array_t tokens = plang_array_new(4096);
        if (tokens == NULL) goto error;
        parser->_tokens = tokens;
        parser->_tokens_count = 0;
        parser->_token_pos = 0;

        plang_array_t scopes = plang_array_new(4);
        if (scopes == NULL) goto error;
        parser->_scopes = scopes;

        plang_array_t errors = plang_array_new(4);
        if (errors == NULL) goto error;
        parser->_errors = errors;

        plang_dictionary_t units = plang_dictionary_new(4);
        if (units == NULL) goto error;
        parser->_units = units;
        plang_dictionary_set_key_comparator(parser->_units,
                                            plang_token_identifier_comparator,
                                            NULL);

        parser->_program = NULL;
    }

    return parser;

error:
    plang_parser_free(parser);
    return NULL;
}

void
plang_parser_free(plang_parser_t PLANG_NULLABLE parser)
{
    if (parser == NULL) return;

    /* Sources are only referenced by the parser, not owned by it. */

    parser->_preamble_source = NULL;
    plang_array_free(parser->_sources);

    if (parser->_tokens) {
        for (size_t i = 0; i < parser->_tokens_count; i++) {
            plang_token_t token = plang_array_get_item(parser->_tokens,
                                                       i);
            plang_token_free(token);
        }
    }
    plang_array_free(parser->_tokens);

    /* Scopes are only referenced by the parser, not owned by it. */
    plang_array_free(parser->_scopes);

    if (parser->_errors) {
        const size_t count = plang_array_get_count(parser->_errors);
        for (size_t i = 0; i < count; i++) {
            plang_error_t error = plang_array_get_item(parser->_errors,
                                                       i);
            plang_error_free(error);
        }
    }
    plang_array_free(parser->_errors);

    /* Units and the program are owned by the parser. */

    plang_unit_free(parser->_preamble_unit);
    plang_array_t units
        = plang_dictionary_copy_all_values(parser->_units);
    assert(units != NULL);
    const size_t units_count = plang_array_get_count(units);
    for (size_t i = 0; i < units_count; i++) {
        plang_unit_t unit = plang_array_get_item(units, i);
        plang_unit_free(unit);
    }
    plang_program_free(parser->_program);

    free(parser);
}


bool
plang_parser_tokenize(plang_parser_t parser)
{
    plang_token_t token = NULL;
    do {
        token = plang_next_token(parser);
        if (token) {
            bool appended = plang_array_append(parser->_tokens,
                                                       token);
            if (appended == false) return false;
        }
    } while (token != NULL);

    parser->_tokens_count = plang_array_get_count(parser->_tokens);

    return true;
}


bool
plang_parser_setup(plang_parser_t parser)
{
    /*
     This for situations where the tokenizer will be used directly, so
     select the first source.
     */

    plang_source_t source = plang_array_get_item(parser->_sources, 0);
    parser->_source = source;
    parser->_source_pos = 0;

    return true;
}


void
plang_parser_set_preamble(plang_parser_t parser,
                          plang_source_t PLANG_NULLABLE source)
{
    if (parser->_preamble_unit) {
        plang_unit_free(parser->_preamble_unit);
        parser->_preamble_unit = NULL;
    }

    if (parser->_preamble_source) {
        plang_source_free(parser->_preamble_source);
    }
    parser->_preamble_source = source;
}


plang_node_t PLANG_NULLABLE
plang_parser_run_once(plang_parser_t parser,
                      plang_source_t source)
{
    plang_node_t top_node = NULL;

    parser->_source = source;
    parser->_source_pos = 0;

    /*
     A run of the parser does not push or pop scopes, because that's
     handled by the UNIT and PROGRAM parsers themselves.
     */

    bool tokenized = plang_parser_tokenize(parser);
    assert(tokenized != false);

    top_node = plang_node_topmost_parse(parser);

    return top_node;
}


bool
plang_parser_run(plang_parser_t parser)
{
    /*
     If there's a preamble set, ensure it's parsed as a unit and its
     interface scope is the base of the scope stack.
     */

    if ((parser->_preamble_unit == NULL) &&
        (parser->_preamble_source != NULL))
    {
        plang_node_t preamble_unit_node
            = plang_parser_run_once(parser, parser->_preamble_source);
        if ((preamble_unit_node == NULL) ||
            (plang_array_get_count(parser->_errors) > 0))
        {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_UNIT,
                                  parser->_preamble_source,
                                  plang_range(0, 0));
            plang_parser_signal_error(parser, error);
            return false;
        }

        if (plang_node_get_type(preamble_unit_node) !=
            plang_node_type_unit)
        {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_UNIT,
                                  parser->_preamble_source,
                                  plang_range(0, 0));
            plang_parser_signal_error(parser, error);

            plang_node_free(preamble_unit_node);

            return false;
        }

        plang_token_t preamble_unit_identifier
            = plang_node_unit_get_identifier(preamble_unit_node);
        parser->_preamble_unit
            = plang_unit_new(parser,
                             preamble_unit_identifier,
                             preamble_unit_node);
        assert(parser->_preamble_unit != NULL);

        plang_scope_t preamble_scope
            = plang_node_unit_get_interface_scope(preamble_unit_node);

        bool pushed_scope = plang_parser_scope_push(parser,
                                                    preamble_scope);
        if (pushed_scope == false) {
            plang_node_free(preamble_unit_node);
            parser->_preamble_unit = NULL;

            return false;
        }
    }

    /* Now parse all the sources passed. */

    const size_t sources_count
        = plang_array_get_count(parser->_sources);

    for (size_t sources_idx = 0;
         sources_idx < sources_count;
         sources_idx++)
    {
        plang_source_t source = plang_array_get_item(parser->_sources,
                                                     sources_idx);

        plang_node_t top_node = plang_parser_run_once(parser, source);
        if (top_node == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_PROGRAM_or_UNIT,
                                  parser->_source,
                                  plang_range(0, 0));
            plang_parser_signal_error(parser, error);
            return false;
        }

        /* Save the parsed PROGRAM or UNIT. */

        switch (plang_node_get_type(top_node)) {
            case plang_node_type_program: {
                // TODO: Real error for multiple PROGRAMs
                assert(parser->_program == NULL);
                plang_token_t name
                    = plang_node_program_get_identifier(top_node);
                plang_program_t program = plang_program_new(parser,
                                                            name,
                                                            top_node);
                assert(program != NULL);
                parser->_program = program;
            } break;

            case plang_node_type_unit: {
                plang_token_t identifier
                    = plang_node_unit_get_identifier(top_node);
                plang_unit_t unit = plang_unit_new(parser,
                                                   identifier,
                                                   top_node);
                assert(unit != NULL);
                bool did_set = plang_dictionary_set(parser->_units,
                                                    identifier,
                                                    unit);
                assert(did_set != false);
            } break;

            default: {
                /* Should never get here. */
                assert(false);
            } break;
        }
    }

    return true;
}


bool
plang_parser_teardown(plang_parser_t parser)
{
    /*
     Nothing actually needed right now for teardown. (Resources are
     managed by the parser instance itself.)
     */

    return true;
}


plang_token_t PLANG_NULLABLE
plang_parser_next_absolute_token(plang_parser_t parser)
{
    plang_token_t token = NULL;

    /* Return the next token, no matter what it is, and advance. */

    if (parser->_token_pos < parser->_tokens_count) {
        token = plang_array_get_item(parser->_tokens,
                                     parser->_token_pos);
        parser->_token_pos++;
    }

    return token;
}


plang_token_t PLANG_NULLABLE
plang_parser_next_significant_token(plang_parser_t parser)
{
    plang_token_t token = NULL;

    /*
     Skip forward to the next non-whitespace, non-comment token or to
     the end of the collection, whichever comes first.
     */

    do {
        token = plang_array_get_item(parser->_tokens,
                                     parser->_token_pos);
        parser->_token_pos++;
    } while ((parser->_token_pos < parser->_tokens_count) &&
             ((plang_token_get_type(token) == plang_token_type_whitespace) ||
              (plang_token_get_type(token) == plang_token_type_parentheses_comment) ||
              (plang_token_get_type(token) == plang_token_type_brace_comment)));

    /* If we didn't find a non-comment token, be sure to return NULL. */

    if ((plang_token_get_type(token) == plang_token_type_whitespace) ||
        (plang_token_get_type(token) == plang_token_type_parentheses_comment) ||
        (plang_token_get_type(token) == plang_token_type_brace_comment))
    {
        token = NULL;
    }

    return token;
}


void
plang_parser_return_token(plang_parser_t parser,
                          plang_token_t token)
{
    parser->_token_pos -= 1;

    assert(token == plang_array_get_item(parser->_tokens,
                                         parser->_token_pos));
}


plang_source_t
plang_parser_get_source(plang_parser_t parser)
{
    return parser->_source;
}


size_t
plang_parser_get_position(plang_parser_t parser)
{
    plang_token_t token = plang_array_get_item(parser->_tokens,
                                               parser->_token_pos);

    plang_range_t range = plang_token_get_range(token);

    return range.start;
}


/* MARK: - Scope Management */

bool
plang_parser_scope_push(plang_parser_t parser,
                        plang_scope_t scope)
{
    bool appended = plang_array_append(parser->_scopes, scope);
    if (appended) {
        parser->_current_scope = scope;
    }

    return appended;
}


plang_scope_t PLANG_NULLABLE
plang_parser_scope_pop(plang_parser_t parser)
{
    plang_array_remove_last_item(parser->_scopes);

    const size_t count = plang_array_get_count(parser->_scopes);
    plang_scope_t current_scope
        = (count > 0) ? plang_array_get_last_item(parser->_scopes)
                      : NULL;

    parser->_current_scope = current_scope;

    return current_scope;
}


plang_scope_t
plang_parser_scope_current(plang_parser_t parser)
{
    return parser->_current_scope;
}


/* MARK: - Error Handling */

void
plang_parser_signal_error(plang_parser_t parser,
                          plang_error_t error)
{
    plang_array_t errors = parser->_errors;
    bool appended = plang_array_append(errors, error);
    assert(appended != false);
}


plang_array_t PLANG_NULLABLE
plang_parser_copy_errors(plang_parser_t parser)
{
    plang_array_t errors = parser->_errors;
    const size_t count = plang_array_get_count(errors);
    if (count > 0) return plang_array_copy(errors);
    else return NULL;
}


/* MARK: - Result Handling */

plang_program_t PLANG_NULLABLE
plang_parser_get_program(plang_parser_t parser)
{
    return parser->_program;
}


plang_unit_t PLANG_NULLABLE
plang_parser_get_unit(plang_parser_t parser,
                      plang_token_t identifier)
{
    return plang_dictionary_get(parser->_units,
                                identifier);
}


plang_dictionary_t PLANG_NULLABLE
plang_parser_copy_all_units(plang_parser_t parser)
{
    return plang_dictionary_copy(parser->_units);
}


/* MARK: - Character Stream Access */

char
plang_get_char(plang_parser_t parser)
{
    char ch = plang_peek_char(parser);
    if (ch != '\0') parser->_source_pos += 1;
    return ch;
}

char
plang_peek_char(plang_parser_t parser)
{
    return plang_source_get_char(parser->_source, parser->_source_pos);
}

void
plang_unget_char(plang_parser_t parser,
                 char ch)
{
    assert(parser->_source_pos > 0);

    const size_t source_len = plang_source_get_length(parser->_source);
    if ((parser->_source_pos == source_len) && (ch == '\0')) {
        /* If we're at end of text, stay there. */
    } else {
        parser->_source_pos -= 1;

        assert(plang_peek_char(parser) == ch);
    }
}


PLANG_SOURCE_END
