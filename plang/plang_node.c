/*
    plang_node.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_node_internal.h"

#include <assert.h>
#include <stdlib.h>

#include "plang_array.h"
#include "plang_constant.h"
#include "plang_error.h"
#include "plang_function.h"
#include "plang_parser.h"
#include "plang_procedure.h"
#include "plang_scope.h"
#include "plang_token.h"
#include "plang_type.h"
#include "plang_variable.h"

PLANG_SOURCE_BEGIN


plang_node_t PLANG_NULLABLE
plang_node_new(plang_node_type_t type)
{
    plang_node_t node = NULL;
    size_t size;

    /*
     WARNING: Do not add a default clause to the below switch construct,
              that way new node types immediately generate a warning.
     */

    switch (type) {
        case plang_node_type_unknown:
            /* Should never get here. */
            assert(false);
            break;

/*! Convenience macro for many nearly-identical cases. */
#define PLANG_NODE_SIZE(t) \
    plang_node_type_ ## t: \
    size = sizeof(struct plang_node_ ## t); \
    break

            /* Constants */
        case PLANG_NODE_SIZE(unsigned_integer);
        case PLANG_NODE_SIZE(unsigned_real);
        case PLANG_NODE_SIZE(scale_factor);
        case PLANG_NODE_SIZE(unsigned_number);
        case PLANG_NODE_SIZE(signed_number);
        case PLANG_NODE_SIZE(constant_declaration);
        case PLANG_NODE_SIZE(constant);
        case PLANG_NODE_SIZE(constant_identifier);
        case PLANG_NODE_SIZE(sign);

            /* Blocks */
        case PLANG_NODE_SIZE(block);
        case PLANG_NODE_SIZE(label_declaration_part);
        case PLANG_NODE_SIZE(label);
        case PLANG_NODE_SIZE(constant_declaration_part);
        case PLANG_NODE_SIZE(type_declaration_part);
        case PLANG_NODE_SIZE(variable_declaration_part);
        case PLANG_NODE_SIZE(procedure_and_function_declaration_part);
        case PLANG_NODE_SIZE(statement_part);

            /* Types */
        case PLANG_NODE_SIZE(type_declaration);
        case PLANG_NODE_SIZE(type_identifier);
        case PLANG_NODE_SIZE(string_type);
        case PLANG_NODE_SIZE(enumerated_type);
        case PLANG_NODE_SIZE(identifier_list);
        case PLANG_NODE_SIZE(subrange_type);
        case PLANG_NODE_SIZE(array_type);
        case PLANG_NODE_SIZE(index_type);
        case PLANG_NODE_SIZE(set_type);
            /* No file type. */
        case PLANG_NODE_SIZE(record_type);
        case PLANG_NODE_SIZE(field_list);
        case PLANG_NODE_SIZE(field_declaration);
        case PLANG_NODE_SIZE(fixed_part);
        case PLANG_NODE_SIZE(variant_part);
        case PLANG_NODE_SIZE(variant);
        case PLANG_NODE_SIZE(tag_field_type);
        case PLANG_NODE_SIZE(pointer_type);

            /* Variables */
        case PLANG_NODE_SIZE(variable_declaration);
        case PLANG_NODE_SIZE(variable_reference);
        case PLANG_NODE_SIZE(variable_identifier);
        case PLANG_NODE_SIZE(index);
        case PLANG_NODE_SIZE(field_designator);
            /* No file type. */
        case PLANG_NODE_SIZE(pointer_object_symbol);

            /* Expressions */
        case PLANG_NODE_SIZE(expression);
        case PLANG_NODE_SIZE(address_of);
        case PLANG_NODE_SIZE(unsigned_constant);
        case PLANG_NODE_SIZE(parenthesized_expression);
        case PLANG_NODE_SIZE(antifactor);
        case PLANG_NODE_SIZE(term);
        case PLANG_NODE_SIZE(simple_expression);
        case PLANG_NODE_SIZE(function_call);
        case PLANG_NODE_SIZE(actual_parameter_list);
        case PLANG_NODE_SIZE(actual_parameter);
        case PLANG_NODE_SIZE(set_constructor);
        case PLANG_NODE_SIZE(member_group);

            /* Statements */
        case PLANG_NODE_SIZE(statement);
        case PLANG_NODE_SIZE(statement_label);
        case PLANG_NODE_SIZE(assignment_statement);
        case PLANG_NODE_SIZE(procedure_statement);
        case PLANG_NODE_SIZE(goto_statement);
        case PLANG_NODE_SIZE(compound_statement);
        case PLANG_NODE_SIZE(if_statement);
        case PLANG_NODE_SIZE(case_statement);
        case PLANG_NODE_SIZE(case_clause);
        case PLANG_NODE_SIZE(otherwise_clause);
        case PLANG_NODE_SIZE(repeat_statement);
        case PLANG_NODE_SIZE(while_statement);
        case PLANG_NODE_SIZE(for_statement);
        case PLANG_NODE_SIZE(control_variable);
        case PLANG_NODE_SIZE(initial_value);
        case PLANG_NODE_SIZE(final_value);
        case PLANG_NODE_SIZE(record_variable_reference);
        case PLANG_NODE_SIZE(with_statement);

            /* Procedures & Functions */
        case PLANG_NODE_SIZE(procedure_identifier);
        case PLANG_NODE_SIZE(function_identifier);
        case PLANG_NODE_SIZE(procedure_reference);
        case PLANG_NODE_SIZE(function_reference);
        case PLANG_NODE_SIZE(procedure_declaration);
        case PLANG_NODE_SIZE(procedure_heading);
        case PLANG_NODE_SIZE(procedure_body);
        case PLANG_NODE_SIZE(function_declaration);
        case PLANG_NODE_SIZE(function_heading);
        case PLANG_NODE_SIZE(result_type);
        case PLANG_NODE_SIZE(function_body);
        case PLANG_NODE_SIZE(formal_parameter_list);
        case PLANG_NODE_SIZE(parameter_declaration);

            /* Programs */
        case PLANG_NODE_SIZE(program);
        case PLANG_NODE_SIZE(program_heading);
        case PLANG_NODE_SIZE(uses_clause);

            /* Units */
        case PLANG_NODE_SIZE(unit);
        case PLANG_NODE_SIZE(unit_heading);
        case PLANG_NODE_SIZE(interface_part);
        case PLANG_NODE_SIZE(implementation_part);
        case PLANG_NODE_SIZE(subroutine_part);

#undef PLANG_NODE_SIZE
    }

    assert(size > sizeof(struct plang_node));

    node = calloc(size, 1);
    if (node) {
        node->_type = type;
    }

    return node;
}


/*!
 Convenience macro for allocating node structs.

 A convenience macro for node internals that avoids casting.
 */
#define PLANG_NODE_NEW(t) \
    (struct plang_node_ ## t *) plang_node_new(plang_node_type_ ## t)


void
plang_node_free(plang_node_t PLANG_NULLABLE node)
{
    if (node == NULL) return;

    /*
     WARNING: Tokens are owned by the parser, not by the node(s) that
              reference them. Thus they MUST NOT be freed here.

     WARNING: Do not add a default clause to the below switch construct,
              that way new node types immediately generate a warning.
     */

    switch (node->_type) {

        case plang_node_type_unknown: {
            /* Should never get here. */
            assert(false);
        } break;

/*! Convenience macro for many nearly-identical cases. */
#define PLANG_NODE_FREE(t) \
    plang_node_type_ ## t: \
    plang_node_ ## t ## _free((struct plang_node_ ## t *) node) ; \
    break

            /* Constants */
        case PLANG_NODE_FREE(unsigned_integer);
        case PLANG_NODE_FREE(unsigned_real);
        case PLANG_NODE_FREE(scale_factor);
        case PLANG_NODE_FREE(unsigned_number);
        case PLANG_NODE_FREE(signed_number);
        case PLANG_NODE_FREE(constant_declaration);
        case PLANG_NODE_FREE(constant);
        case PLANG_NODE_FREE(constant_identifier);
        case PLANG_NODE_FREE(sign);

            /* Blocks */
        case PLANG_NODE_FREE(block);
        case PLANG_NODE_FREE(label_declaration_part);
        case PLANG_NODE_FREE(label);
        case PLANG_NODE_FREE(constant_declaration_part);
        case PLANG_NODE_FREE(type_declaration_part);
        case PLANG_NODE_FREE(variable_declaration_part);
        case PLANG_NODE_FREE(procedure_and_function_declaration_part);
        case PLANG_NODE_FREE(statement_part);

            /* Types */
        case PLANG_NODE_FREE(type_declaration);
        case PLANG_NODE_FREE(type_identifier);
        case PLANG_NODE_FREE(string_type);
        case PLANG_NODE_FREE(enumerated_type);
        case PLANG_NODE_FREE(identifier_list);
        case PLANG_NODE_FREE(subrange_type);
        case PLANG_NODE_FREE(array_type);
        case PLANG_NODE_FREE(index_type);
        case PLANG_NODE_FREE(set_type);
            /* No file type. */
        case PLANG_NODE_FREE(record_type);
        case PLANG_NODE_FREE(field_list);
        case PLANG_NODE_FREE(field_declaration);
        case PLANG_NODE_FREE(fixed_part);
        case PLANG_NODE_FREE(variant_part);
        case PLANG_NODE_FREE(variant);
        case PLANG_NODE_FREE(tag_field_type);
        case PLANG_NODE_FREE(pointer_type);

            /* Variables */
        case PLANG_NODE_FREE(variable_declaration);
        case PLANG_NODE_FREE(variable_reference);
        case PLANG_NODE_FREE(variable_identifier);
        case PLANG_NODE_FREE(index);
        case PLANG_NODE_FREE(field_designator);
            /* No file type. */
        case PLANG_NODE_FREE(pointer_object_symbol);

            /* Expressions */
        case PLANG_NODE_FREE(expression);
        case PLANG_NODE_FREE(address_of);
        case PLANG_NODE_FREE(unsigned_constant);
        case PLANG_NODE_FREE(parenthesized_expression);
        case PLANG_NODE_FREE(antifactor);
        case PLANG_NODE_FREE(term);
        case PLANG_NODE_FREE(simple_expression);
        case PLANG_NODE_FREE(function_call);
        case PLANG_NODE_FREE(actual_parameter_list);
        case PLANG_NODE_FREE(actual_parameter);
        case PLANG_NODE_FREE(set_constructor);
        case PLANG_NODE_FREE(member_group);

            /* Statements */
        case PLANG_NODE_FREE(statement);
        case PLANG_NODE_FREE(statement_label);
        case PLANG_NODE_FREE(assignment_statement);
        case PLANG_NODE_FREE(procedure_statement);
        case PLANG_NODE_FREE(goto_statement);
        case PLANG_NODE_FREE(compound_statement);
        case PLANG_NODE_FREE(if_statement);
        case PLANG_NODE_FREE(case_statement);
        case PLANG_NODE_FREE(case_clause);
        case PLANG_NODE_FREE(otherwise_clause);
        case PLANG_NODE_FREE(repeat_statement);
        case PLANG_NODE_FREE(while_statement);
        case PLANG_NODE_FREE(for_statement);
        case PLANG_NODE_FREE(control_variable);
        case PLANG_NODE_FREE(initial_value);
        case PLANG_NODE_FREE(final_value);
        case PLANG_NODE_FREE(record_variable_reference);
        case PLANG_NODE_FREE(with_statement);

            /* Procedures & Functions */
        case PLANG_NODE_FREE(procedure_identifier);
        case PLANG_NODE_FREE(function_identifier);
        case PLANG_NODE_FREE(procedure_reference);
        case PLANG_NODE_FREE(function_reference);
        case PLANG_NODE_FREE(procedure_declaration);
        case PLANG_NODE_FREE(procedure_heading);
        case PLANG_NODE_FREE(procedure_body);
        case PLANG_NODE_FREE(function_declaration);
        case PLANG_NODE_FREE(function_heading);
        case PLANG_NODE_FREE(result_type);
        case PLANG_NODE_FREE(function_body);
        case PLANG_NODE_FREE(formal_parameter_list);
        case PLANG_NODE_FREE(parameter_declaration);

            /* Programs */
        case PLANG_NODE_FREE(program);
        case PLANG_NODE_FREE(program_heading);
        case PLANG_NODE_FREE(uses_clause);

            /* Units */
        case PLANG_NODE_FREE(unit);
        case PLANG_NODE_FREE(unit_heading);
        case PLANG_NODE_FREE(interface_part);
        case PLANG_NODE_FREE(implementation_part);
        case PLANG_NODE_FREE(subroutine_part);

#undef PLANG_NODE_FREE
    }

    free(node);
}


plang_node_type_t
plang_node_get_type(plang_node_t node)
{
    return node->_type;
}


/*!
 Fully dispose of a ``plang_node_t`` array.

 Frees all of the nodes in the array and then frees the array.
 */
void
plang_node_array_free(plang_array_t PLANG_NULLABLE array)
{
    if (array == NULL) return;

    const size_t count = plang_array_get_count(array);
    for (size_t i = 0; i < count; i++) {
        plang_node_t item = plang_array_get_item(array, i);
        plang_node_free(item);
    }

    plang_array_free(array);
}


plang_node_t PLANG_NULLABLE
plang_node_topmost_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_unit_parse(parser);
    if (node) return node;

    node = plang_node_program_parse(parser);
    if (node) return node;

    return node;
}


/* MARK: - Constants */

plang_node_t PLANG_NULLABLE
plang_node_unsigned_integer_parse(plang_parser_t parser)
{
    struct plang_node_unsigned_integer *node = NULL;

    plang_token_t digit_sequence = NULL;
    plang_token_t hex_digit_sequence = NULL;
    plang_token_t token = plang_parser_next_significant_token(parser);
    if (plang_token_matches(token, plang_token_type_digit_sequence)) {
        digit_sequence = token;
    } else if (plang_token_matches(token,
                                   plang_token_type_hex_digit_sequence))
    {
        hex_digit_sequence = token;
    } else {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, token);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(unsigned_integer);
    assert(node != NULL);

    node->_digit_sequence = digit_sequence;
    node->_hex_digit_sequence = hex_digit_sequence;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unsigned_integer_free(struct plang_node_unsigned_integer *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_unsigned_real_parse(plang_parser_t parser)
{
    struct plang_node_unsigned_real *node = NULL;

    plang_token_t whole_part
        = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(whole_part,
                             plang_token_type_digit_sequence))
    {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, whole_part);
        goto bail_out;
    }

    plang_token_t period = plang_parser_next_absolute_token(parser);
    if (!plang_token_matches(period, plang_token_type_PERIOD)) {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, period);
        plang_parser_return_token(parser, whole_part);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(unsigned_real);
    assert(node != NULL);

    node->_whole_part = whole_part;

    if (period) {
        plang_token_t fractional_part
            = plang_parser_next_absolute_token(parser);
        if (!plang_token_matches(fractional_part,
                                 plang_token_type_digit_sequence))
        {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_digit_sequence,
                                  plang_token_get_source(fractional_part),
                                  plang_token_get_range(fractional_part));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
        node->_fractional_part = fractional_part;
    }

    node->_scale_factor = plang_node_scale_factor_parse(parser);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unsigned_real_free(struct plang_node_unsigned_real *ns)
{
    plang_node_free(ns->_scale_factor);
}

plang_node_t PLANG_NULLABLE
plang_node_scale_factor_parse(plang_parser_t parser)
{
    struct plang_node_scale_factor *node = NULL;

    /*
     There's no separate E/e token for just this one special case;
     instead, it's tokenized as an identifier.

     Also, a scale factor is never used outside the context of parsing a
     a real number, and so must not allow leading whitespace.
     */

    bool isExp = false;
    plang_token_t exp = plang_parser_next_absolute_token(parser);
    if (plang_token_matches(exp, plang_token_type_identifier)) {
        isExp = plang_token_identifier_equals(exp, "E");
    }
    if (isExp == false) {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, exp);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(scale_factor);

    node->_sign = plang_node_sign_parse(parser, false);
    /* Optional. */

    plang_token_t digit_sequence = plang_parser_next_absolute_token(parser);
    if (!plang_token_matches(digit_sequence, plang_token_type_digit_sequence)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_digit_sequence,
                              plang_token_get_source(digit_sequence),
                              plang_token_get_range(digit_sequence));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_digit_sequence = digit_sequence;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_scale_factor_free(struct plang_node_scale_factor *ns)
{
    plang_node_free(ns->_sign);
}

plang_node_t PLANG_NULLABLE
plang_node_unsigned_number_parse(plang_parser_t parser)
{
    struct plang_node_unsigned_number *node = NULL;

    node = PLANG_NODE_NEW(unsigned_number);
    assert(node != NULL);

    /*
     In order to not mistake an integer for a real, parsing the real
     must be attempted first: Both start with a digit-sequence, but the
     real expects both a whole and fractional part with a decimal point
     in between.
     */

    plang_node_t unsigned_real
        = plang_node_unsigned_real_parse(parser);
    if (unsigned_real != NULL) {
        node->_unsigned_real = unsigned_real;
    } else {
        plang_node_t unsigned_integer
            = plang_node_unsigned_integer_parse(parser);
        if (unsigned_integer != NULL) {
            node->_unsigned_integer = unsigned_integer;
        } else {
            /* Not what we're looking for, bail out to backtrack. */
            goto bail_out;
        }
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unsigned_number_free(struct plang_node_unsigned_number *ns)
{
    plang_node_free(ns->_unsigned_integer);
    plang_node_free(ns->_unsigned_real);
}

plang_node_t PLANG_NULLABLE
plang_node_signed_number_parse(plang_parser_t parser)
{
    struct plang_node_signed_number *node = NULL;

    node = PLANG_NODE_NEW(signed_number);
    assert(node != NULL);

    node->_sign = plang_node_sign_parse(parser, true);
    node->_unsigned_number = plang_node_unsigned_number_parse(parser);
    if (node->_unsigned_number == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        if (node->_sign) plang_node_sign_unparse(parser, node->_sign);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_signed_number_free(struct plang_node_signed_number *ns)
{
    plang_node_free(ns->_sign);
    plang_node_free(ns->_unsigned_number);
}

plang_node_t PLANG_NULLABLE
plang_node_constant_declaration_parse(plang_parser_t parser)
{
    struct plang_node_constant_declaration *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(constant_declaration);
    assert(node != NULL);

    node->_identifier = identifier;

    plang_token_t equals = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(equals, plang_token_type_EQUALS)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_EQUALS,
                              plang_parser_get_source(parser),
                              plang_token_get_range(equals));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t constant_node_start = plang_parser_get_position(parser);
    plang_node_t constant_node = plang_node_constant_parse(parser);
    if (constant_node == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_constant,
                              plang_parser_get_source(parser),
                              plang_range(constant_node_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_constant = constant_node;

    plang_token_t semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_token_get_source(semicolon),
                              plang_token_get_range(semicolon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /*
     Register the constant with the current scope. Generate an error if
     the constant already exists within the current scope.
     */
    
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_constant_t constant = plang_constant_new(parser,
                                                   identifier,
                                                   (plang_node_t) node);
    bool registered = plang_scope_constant_register(scope, constant);
    if (registered == false) {
        plang_error_t error
            = plang_error_new(plang_error_type_already_exists_constant,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_constant_declaration_free(struct plang_node_constant_declaration *ns)
{
    plang_node_free(ns->_constant);
}

plang_node_t PLANG_NULLABLE
plang_node_constant_parse(plang_parser_t parser)
{
    struct plang_node_constant *node = NULL;

    node = PLANG_NODE_NEW(constant);
    assert(node != NULL);

    plang_node_t sign = plang_node_sign_parse(parser, true);
    /* Optional */

    plang_node_t constant_identifier = plang_node_constant_identifier_parse(parser);
    if (constant_identifier == NULL) {
        if (sign != NULL) {
            /* Put the sign back and continue. */
            plang_node_sign_unparse(parser, sign);
        }
    } else {
        node->_sign = sign;
        node->_constant_identifier = constant_identifier;
        goto done;
    }

    node->_signed_number = plang_node_signed_number_parse(parser);
    if (node->_signed_number != NULL) goto done;

    plang_token_t token = plang_parser_next_significant_token(parser);
    if (plang_token_matches(token, plang_token_type_string_literal)) {
        node->_quoted_string = token;
        goto done;
    } else {
        plang_parser_return_token(parser, token);
    }

    /* Not what we're looking for, bail out to backtrack. */
    goto bail_out;

done:
    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_constant_free(struct plang_node_constant *ns)
{
    plang_node_free(ns->_sign);
    plang_node_free(ns->_constant_identifier);

    plang_node_free(ns->_signed_number);
}

plang_node_t PLANG_NULLABLE
plang_node_constant_identifier_parse(plang_parser_t parser)
{
    struct plang_node_constant_identifier *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     Look up the constant in the current scope and all parent scopes.
     If no such constant exists, then back out.
     */
    
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_constant_t constant = plang_scope_constant_lookup(scope,
                                                            identifier,
                                                            true);
    if (constant == NULL) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }
    
    node = PLANG_NODE_NEW(constant_identifier);
    assert(node != NULL);

    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_constant_identifier_free(struct plang_node_constant_identifier *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_sign_parse(plang_parser_t parser,
                      bool allow_whitespace)
{
    struct plang_node_sign *node = NULL;

    node = PLANG_NODE_NEW(sign);
    assert(node != NULL);

    /*
     The allow_whitespace parameter is used because a sign within a real
     number's scale factor must not have leading whitespace.
     */

    plang_token_t token;
    if (allow_whitespace) {
        token = plang_parser_next_significant_token(parser);
    } else {
        token = plang_parser_next_absolute_token(parser);
    }

    if (plang_token_matches(token, plang_token_type_PLUS) ||
        plang_token_matches(token, plang_token_type_MINUS))
    {
        node->_token = token;
    } else {
        /* Not what we're looking for, bail out to backtrack. */
        if (token) plang_parser_return_token(parser, token);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_sign_unparse(plang_parser_t parser,
                        plang_node_t node)
{
    struct plang_node_sign *ns = (void *)node;

    if (ns->_token) plang_parser_return_token(parser, ns->_token);
}

void
plang_node_sign_free(struct plang_node_sign *ns)
{
    /* No sub-nodes. */
}


/* MARK: - Blocks */

plang_node_t PLANG_NULLABLE
plang_node_block_parse(plang_parser_t parser)
{
    struct plang_node_block *node = NULL;

    node = PLANG_NODE_NEW(block);
    assert(node != NULL);

    node->_label_declaration_part = plang_node_label_declaration_part_parse(parser);
    node->_constant_declaration_part = plang_node_constant_declaration_part_parse(parser);
    node->_type_declaration_part = plang_node_type_declaration_part_parse(parser);
    node->_variable_declaration_part = plang_node_variable_declaration_part_parse(parser);
    node->_procedure_and_function_declaration_part = plang_node_procedure_and_function_declaration_part_parse(parser, true);
    node->_statement_part = plang_node_statement_part_parse(parser);

    /* If there was nothing in the block, bail out to backtrack. */
    if ((node->_label_declaration_part == NULL) &&
        (node->_constant_declaration_part == NULL) &&
        (node->_type_declaration_part == NULL) &&
        (node->_variable_declaration_part == NULL) &&
        (node->_procedure_and_function_declaration_part == NULL) &&
        (node->_statement_part == NULL))
    {
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_block_free(struct plang_node_block *ns)
{
    plang_node_free(ns->_label_declaration_part);
    plang_node_free(ns->_constant_declaration_part);
    plang_node_free(ns->_type_declaration_part);
    plang_node_free(ns->_variable_declaration_part);
    plang_node_free(ns->_procedure_and_function_declaration_part);
    plang_node_free(ns->_statement_part);
}


plang_node_t PLANG_NULLABLE
plang_node_label_declaration_part_parse(plang_parser_t parser)
{
    struct plang_node_label_declaration_part *node = NULL;

    plang_token_t maybe_label = plang_parser_next_significant_token(parser);
    if (!maybe_label ||
        (plang_token_get_type(maybe_label) != plang_token_type_LABEL))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_label) plang_parser_return_token(parser, maybe_label);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(label_declaration_part);
    assert(node != NULL);

    plang_array_t labels = plang_array_new(4);
    assert(labels != NULL);

    node->_labels = labels;

    bool done = false;
    while (!done) {
        const size_t label_start = plang_parser_get_position(parser);
        plang_node_t label = plang_node_label_parse(parser);
        if (label == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_label,
                                  plang_parser_get_source(parser),
                                  plang_range(label_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        } else {
            bool appended = plang_array_append(labels, label);
            assert(appended != false);
        }

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_COMMA)) {
            /* Keep iterating. */
        } else if (plang_token_matches(token,
                                       plang_token_type_SEMICOLON))
        {
            /* Done, stop iterating. */
            done = true;
        } else {
            /*
             Put the token back and keep iterating, in case it actually
             starts the next label.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_label_declaration_part_free(struct plang_node_label_declaration_part *ns)
{
    plang_node_array_free(ns->_labels);
}

plang_node_t PLANG_NULLABLE
plang_node_label_parse(plang_parser_t parser)
{
    struct plang_node_label *label = NULL;

    plang_token_t token = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(token, plang_token_type_digit_sequence)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (token) plang_parser_return_token(parser, token);
        goto bail_out;
    }

    label = PLANG_NODE_NEW(label);
    assert(label != NULL);

    label->_digit_sequence = token;

    return (plang_node_t) label;

bail_out:
    plang_node_free((plang_node_t) label);
    return NULL;
}

void
plang_node_label_free(struct plang_node_label *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_constant_declaration_part_parse(plang_parser_t parser)
{
    struct plang_node_constant_declaration_part *node = NULL;

    plang_token_t maybe_const = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_const, plang_token_type_CONST)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_const) plang_parser_return_token(parser, maybe_const);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(constant_declaration_part);
    assert(node != NULL);

    plang_array_t constant_declarations = NULL;
    constant_declarations = plang_array_new(8);

    node->_constant_declarations = constant_declarations;

    bool done = false;
    while (!done) {
        plang_node_t constant_declaration = NULL;
        constant_declaration = plang_node_constant_declaration_parse(parser);
        if (constant_declaration) {
            bool appended = plang_array_append(constant_declarations,
                                               constant_declaration);
            assert(appended != false);
        } else {
            done = true;
        }
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_constant_declaration_part_free(struct plang_node_constant_declaration_part *ns)
{
    plang_node_array_free(ns->_constant_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_type_declaration_part_parse(plang_parser_t parser)
{
    struct plang_node_type_declaration_part *node = NULL;

    plang_token_t maybe_type = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_type, plang_token_type_TYPE)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_type) plang_parser_return_token(parser, maybe_type);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(type_declaration_part);
    assert(node != NULL);

    plang_array_t type_declarations = NULL;
    type_declarations = plang_array_new(4);
    assert(type_declarations != NULL);

    node->_type_declarations = type_declarations;

    plang_node_t type_declaration = NULL;
    do {
        type_declaration = plang_node_type_declaration_parse(parser);
        if (type_declaration) {
            bool appended = plang_array_append(type_declarations,
                                               type_declaration);
            assert(appended != false);
        }
    } while (type_declaration != NULL);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_type_declaration_part_free(struct plang_node_type_declaration_part *ns)
{
    plang_node_array_free(ns->_type_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_variable_declaration_part_parse(plang_parser_t parser)
{
    struct plang_node_variable_declaration_part *node = NULL;

    plang_token_t maybe_var = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_var, plang_token_type_VAR)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_var) plang_parser_return_token(parser, maybe_var);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(variable_declaration_part);
    assert(node != NULL);

    plang_array_t variable_declarations = NULL;
    variable_declarations = plang_array_new(8);
    assert(variable_declarations != NULL);

    node->_variable_declarations = variable_declarations;

    plang_node_t variable_declaration = NULL;
    do {
        variable_declaration = plang_node_variable_declaration_parse(parser);
        if (variable_declaration) {
            bool appended = plang_array_append(variable_declarations,
                                               variable_declaration);
            assert(appended != false);
        }
    } while (variable_declaration != NULL);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variable_declaration_part_free(struct plang_node_variable_declaration_part *ns)
{
    plang_node_array_free(ns->_variable_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_procedure_and_function_declaration_part_parse(plang_parser_t parser,
                                                         bool allow_body)
{
    struct plang_node_procedure_and_function_declaration_part *node = NULL;
    
    node = PLANG_NODE_NEW(procedure_and_function_declaration_part);
    assert(node != NULL);
    
    plang_array_t pfds = plang_array_new(4);
    assert(pfds != NULL);
    node->_procedure_and_function_declarations = pfds;
    
    bool first_time = true;
    bool done = false;
    do {
        plang_node_t pfd = NULL;

        pfd = plang_node_procedure_declaration_parse(parser,
                                                     allow_body);

        if (pfd == NULL) {
            pfd = plang_node_function_declaration_parse(parser,
                                                        allow_body);
        }

        if (pfd == NULL) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            } else {
                /* Just be done. */
                done = true;
            }
        } else {
            bool appended = plang_array_append(pfds, pfd);
            assert(appended != false);
        }
        if (first_time) first_time = false;
    } while (!done);
    
    return (plang_node_t) node;
    
bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_and_function_declaration_part_free(struct plang_node_procedure_and_function_declaration_part *ns)
{
    plang_node_array_free(ns->_procedure_and_function_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_statement_part_parse(plang_parser_t parser)
{
    struct plang_node_statement_part *node = NULL;

    plang_node_t compound_statement
        = plang_node_compound_statement_parse(parser);
    if (compound_statement == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(statement_part);
    assert(node != NULL);

    node->_compound_statement = compound_statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_statement_part_free(struct plang_node_statement_part *ns)
{
    plang_node_free(ns->_compound_statement);
}


/* MARK: - Types */

plang_node_t PLANG_NULLABLE
plang_node_type_declaration_parse(plang_parser_t parser)
{
    struct plang_node_type_declaration *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    plang_token_t equals = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(equals, plang_token_type_EQUALS)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(equals));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(type_declaration);
    assert(node != NULL);

    node->_identifier = identifier;

    /*
     Register the type with the current scope. Generate an error if
     the type already exists within the current scope.

     This must be done here so the identifier representing the type is
     available for use as a type within its declaration. (The node can
     still be modified after its registration.)
     */

    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type = plang_type_new(parser,
                                       identifier,
                                       (plang_node_t) node);
    bool registered = plang_scope_type_register(scope, type);
    if (registered == false) {
        plang_error_t error
            = plang_error_new(plang_error_type_already_exists_type,
                              plang_token_get_source(identifier),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t type_node_start = plang_parser_get_position(parser);
    plang_node_t type_node = plang_node_type_parse(parser);
    if (!type_node) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type,
                              plang_parser_get_source(parser),
                              plang_range(type_node_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_type = type_node;

    plang_token_t semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(equals));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /* Ensure the type knows where its declaration comes from. */

    plang_node_type_set_type_declaration(type_node, (plang_node_t) node);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_type_declaration_free(struct plang_node_type_declaration *ns)
{
    plang_node_free(ns->_type);
}

plang_node_t PLANG_NULLABLE
plang_node_type_declaration_get_type(plang_node_t node)
{
    assert(node->_type == plang_node_type_type_declaration);

    struct plang_node_type_declaration *ns = (void *)node;

    return ns->_type;
}

plang_token_t
plang_node_type_declaration_get_identifier(plang_node_t node)
{
    assert(node->_type == plang_node_type_type_declaration);

    struct plang_node_type_declaration *ns = (void *)node;

    return ns->_identifier;
}

void
plang_node_type_set_type_declaration(plang_node_t type_node,
                                     plang_node_t declaration_node)
{
    switch (type_node->_type) {

            /* Simple Types */

        case plang_node_type_subrange_type: {
            struct plang_node_subrange_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;

        case plang_node_type_enumerated_type: {
            struct plang_node_enumerated_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;

        case plang_node_type_string_type: {
            struct plang_node_string_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;


            /* Structured Types */

        case plang_node_type_array_type: {
            struct plang_node_array_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;

        case plang_node_type_set_type: {
            struct plang_node_set_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;

        case plang_node_type_record_type: {
            struct plang_node_record_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;


            /* Type Declarations */

        case plang_node_type_pointer_type: {
            struct plang_node_pointer_type *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;

        case plang_node_type_type_identifier: {
            struct plang_node_type_identifier *ns = (void *)type_node;
            ns->_declaration = declaration_node;
        } break;


            /* Backstop */

        default: {
            assert(false); /* should never get here */
        }
    }
}

plang_node_t PLANG_NULLABLE
plang_node_type_get_type_declaration(plang_node_t type_node)
{
    switch (type_node->_type) {

            /* Simple Types */

        case plang_node_type_subrange_type: {
            struct plang_node_subrange_type *ns = (void *)type_node;
            return ns->_declaration;
        }

        case plang_node_type_enumerated_type: {
            struct plang_node_enumerated_type *ns = (void *)type_node;
            return ns->_declaration;
        }

        case plang_node_type_string_type: {
            struct plang_node_string_type *ns = (void *)type_node;
            return ns->_declaration;
        }


            /* Structured Types */

        case plang_node_type_array_type: {
            struct plang_node_array_type *ns = (void *)type_node;
            return ns->_declaration;
        }

        case plang_node_type_set_type: {
            struct plang_node_set_type *ns = (void *)type_node;
            return ns->_declaration;
        }

        case plang_node_type_record_type: {
            struct plang_node_record_type *ns = (void *)type_node;
            return ns->_declaration;
        }


            /* Type Identifiers */

        case plang_node_type_pointer_type: {
            struct plang_node_pointer_type *ns = (void *)type_node;
            return ns->_declaration;
        }

        case plang_node_type_type_identifier: {
            struct plang_node_type_identifier *ns = (void *)type_node;
            return ns->_declaration;
        }


            /* Backstop */

        default: {
            return NULL;
        }
    }
}

plang_node_t PLANG_NULLABLE
plang_node_type_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_simple_type_parse(parser);
    if (node) return node;

    node = plang_node_structured_type_parse(parser);
    if (node) return node;

    node = plang_node_pointer_type_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_simple_type_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_ordinal_type_parse(parser);
    if (node) return node;

    node = plang_node_real_type_identifier_parse(parser);
    if (node) return node;

    node = plang_node_string_type_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_type_identifier_parse(plang_parser_t parser)
{
    struct plang_node_type_identifier *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Just back out. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     Look up the type in the current scope and all parent scopes.
     If no such type exists, then back out.
     */
    
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type = plang_scope_type_lookup(scope, identifier, true);
    if (type == NULL) {
        /*
         There are special type identifiers that start with the prefix
         __BUILT_IN_ that resolve to themselves. These are handled by
         the type system directly, to implement the built-in types;
         these types are then declared in the preamble.
         */
        if (!plang_token_identifier_is_built_in_type(identifier)) {
            plang_parser_return_token(parser, identifier);
            goto bail_out;
        }
    }

    node = PLANG_NODE_NEW(type_identifier);
    assert(node != NULL);

    node->_identifier = identifier;

    /*
     If this was the first time "__BUILT_IN_" was encountered within
     this scope, register it as a type with the scope using this node,
     in order to ensure future lookups in the same scope will succeed.
     */
    if (type == NULL) {
        plang_type_t builtin_type
            = plang_type_new(parser, identifier, (plang_node_t) node);
        bool registered = plang_scope_type_register(scope,
                                                    builtin_type);
        assert(registered != false);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_type_identifier_free(struct plang_node_type_identifier *ns)
{
    /* No sub-nodes. */
}

plang_token_t
plang_node_type_identifier_get_identifier(plang_node_t PLANG_NULLABLE node)
{
    assert(node->_type == plang_node_type_type_identifier);

    struct plang_node_type_identifier *ns = (void *)node;

    return ns->_identifier;
}


plang_node_t PLANG_NULLABLE
plang_node_simple_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_ordinal_type_identifier_parse(parser);
    if (node) return node;

    node = plang_node_real_type_identifier_parse(parser);
    if (node) return node;

    node = plang_node_string_type_identifier_parse(parser);
    if (node) return node;

    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_structured_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = plang_node_type_identifier_parse(parser);
    if (node == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    /*
     Check whether the identifier references a known structured type.
     */

    plang_token_t identifier
        = plang_node_type_identifier_get_identifier(node);
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type
        = plang_scope_type_lookup(scope, identifier, true);
    if (!plang_type_is_structured(type, scope)) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     The identifier refers to a structured type, so return it.
     */

    return node;

bail_out:
    plang_node_free(node);
    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_pointer_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = plang_node_type_identifier_parse(parser);
    if (node == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    /*
     Check whether the identifier references a known pointer type.
     */

    plang_token_t identifier
        = plang_node_type_identifier_get_identifier(node);
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type
        = plang_scope_type_lookup(scope, identifier, true);
    if (!plang_type_is_pointer(type, scope)) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     The identifier refers to a pointer type, so return it.
     */

    return node;

bail_out:
    plang_node_free(node);
    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_ordinal_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = plang_node_type_identifier_parse(parser);
    if (node == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    /*
     Check whether the identifier references a known ordinal (subrange
     or enumerated) type.
     */

    plang_token_t identifier
        = plang_node_type_identifier_get_identifier(node);
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type
        = plang_scope_type_lookup(scope, identifier, true);
    if (!plang_type_is_ordinal(type, scope)) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     The identifier refers to an ordinal type, so return it.
     */

    return node;

bail_out:
    plang_node_free(node);
    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_real_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = plang_node_type_identifier_parse(parser);
    if (node == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    /*
     Check whether the identifier references a known real type.
     */

    plang_token_t identifier
        = plang_node_type_identifier_get_identifier(node);
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type
        = plang_scope_type_lookup(scope, identifier, true);
    if (!plang_type_is_real(type, scope)) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     The identifier refers to a real type, so return it.
     */

    return node;

bail_out:
    plang_node_free(node);
    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_string_type_identifier_parse(plang_parser_t parser)
{
    plang_node_t node = plang_node_type_identifier_parse(parser);
    if (node == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    /*
     Check whether the identifier references a known string type.
     */

    plang_token_t identifier
        = plang_node_type_identifier_get_identifier(node);
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_type_t type
        = plang_scope_type_lookup(scope, identifier, true);
    if (!plang_type_is_string(type, scope)) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     The identifier refers to a string type, so return it.
     */

    return node;

bail_out:
    plang_node_free(node);
    return NULL;
}


plang_node_t PLANG_NULLABLE
plang_node_ordinal_type_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_subrange_type_parse(parser);
    if (node) return node;

    node = plang_node_enumerated_type_parse(parser);
    if (node) return node;

    node = plang_node_ordinal_type_identifier_parse(parser);
    if (node) return node;

    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_string_type_parse(plang_parser_t parser)
{
    struct plang_node_string_type *node = NULL;

    /* Try string-type-identifier. (Looks up existing node.) */
    plang_node_t string_type_identifier
        = plang_node_string_type_identifier_parse(parser);
    if (string_type_identifier) return string_type_identifier;

    plang_token_t maybe_string = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_string, plang_token_type_STRING)) {
        if (maybe_string) plang_parser_return_token(parser,
                                                    maybe_string);
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(string_type);
    assert(node != NULL);

    plang_token_t lbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(lbracket, plang_token_type_LBRACKET)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_LBRACKET,
                              plang_parser_get_source(parser),
                              plang_token_get_range(lbracket));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t size_attribute_start
        = plang_parser_get_position(parser);
    plang_node_t size_attribute
        = plang_node_unsigned_integer_parse(parser);
    if (size_attribute == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_unsigned_integer,
                              plang_parser_get_source(parser),
                              plang_range(size_attribute_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_size_attribute = size_attribute;

    plang_token_t rbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(rbracket, plang_token_type_RBRACKET)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RBRACKET,
                              plang_parser_get_source(parser),
                              plang_token_get_range(rbracket));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_string_type_free(struct plang_node_string_type *ns)
{
    plang_node_free(ns->_size_attribute);
}

plang_node_t PLANG_NULLABLE
plang_node_enumerated_type_parse(plang_parser_t parser)
{
    struct plang_node_enumerated_type *node = NULL;

    plang_token_t maybe_lparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lparen, plang_token_type_LPAREN)) {
        /* Just bail out to backtrack. */
        if (maybe_lparen) plang_parser_return_token(parser,
                                                    maybe_lparen);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(enumerated_type);
    assert(node != NULL);

    const size_t identifier_list_start = plang_parser_get_position(parser);
    plang_node_t identifier_list = plang_node_identifier_list_parse(parser);
    if (!identifier_list) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier_list,
                              plang_parser_get_source(parser),
                              plang_range(identifier_list_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_identifier_list = identifier_list;

    plang_token_t rparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(rparen, plang_token_type_RPAREN)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RPAREN,
                              plang_parser_get_source(parser),
                              plang_token_get_range(rparen));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_enumerated_type_free(struct plang_node_enumerated_type *ns)
{
    plang_node_free(ns->_identifier_list);
}

plang_node_t PLANG_NULLABLE
plang_node_identifier_list_parse(plang_parser_t parser)
{
    struct plang_node_identifier_list *node = NULL;

    node = PLANG_NODE_NEW(identifier_list);
    assert(node != NULL);

    plang_array_t identifiers = plang_array_new(8);
    assert(identifiers != NULL);

    node->_identifiers = identifiers;

    bool first_time = true;
    bool done = false;
    while (!done) {
        plang_token_t identifier = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(identifier, plang_token_type_identifier)) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                plang_parser_return_token(parser, identifier);
            } else {
                plang_error_t error
                = plang_error_new(plang_error_type_expected_identifier,
                                  plang_parser_get_source(parser),
                                  plang_token_get_range(identifier));
                plang_parser_signal_error(parser, error);
            }
            goto bail_out;
        } else {
            bool appended = plang_array_append(identifiers,
                                               identifier);
            assert(appended != false);

            plang_token_t maybe_comma = plang_parser_next_significant_token(parser);
            if (!plang_token_matches(maybe_comma,
                                     plang_token_type_COMMA)) {
                /*
                 Done with the identifier list, so put back the last
                 requested token and exit the loop.
                 */
                if (maybe_comma) plang_parser_return_token(parser,
                                                           maybe_comma);
                done = true;
            } else {
                /* Keep iterating. */
            }
        }
        first_time = false;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_identifier_list_free(struct plang_node_identifier_list *ns)
{
    /* Array of plang_token_t, just free the array. */
    plang_array_free(ns->_identifiers);
}

plang_node_t PLANG_NULLABLE
plang_node_subrange_type_parse(plang_parser_t parser)
{
    struct plang_node_subrange_type *node = NULL;

    plang_node_t start = plang_node_constant_parse(parser);
    if (start == NULL) {
        /* Just bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(subrange_type);
    assert(node != NULL);

    node->_start = start;

    plang_token_t range = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(range, plang_token_type_RANGE)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RANGE,
                              plang_parser_get_source(parser),
                              plang_token_get_range(range));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t end_start = plang_parser_get_position(parser);
    plang_node_t end = plang_node_constant_parse(parser);
    if (end == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_constant,
                              plang_parser_get_source(parser),
                              plang_range(end_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_end = end;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_subrange_type_free(struct plang_node_subrange_type *ns)
{
    plang_node_free(ns->_start);
    plang_node_free(ns->_end);
}

plang_node_t PLANG_NULLABLE
plang_node_structured_type_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    const size_t position = plang_parser_get_position(parser);

    node = plang_node_structured_type_identifier_parse(parser);
    if (node) goto done;

    bool is_packed = false;
    plang_token_t maybe_packed = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_packed, plang_token_type_PACKED))
    {
        is_packed = true;
    } else {
        if (maybe_packed) plang_parser_return_token(parser,
                                                    maybe_packed);
        /* Don't bail out, PACKED is optional. */
    }

    node = plang_node_array_type_parse(parser, is_packed);
    if (node) return node;

    node = plang_node_set_type_parse(parser, is_packed);
    if (node) return node;

    /* No file type. */

    node = plang_node_record_type_parse(parser, is_packed);
    if (node) return node;

    if (is_packed && (node == NULL)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_array_set_or_record,
                              plang_parser_get_source(parser),
                              plang_range(position, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

done:
    return node;

bail_out:
    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_array_type_parse(plang_parser_t parser,
                            bool is_packed)
{
    struct plang_node_array_type *node = NULL;

    plang_token_t maybe_array = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_array, plang_token_type_ARRAY)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_array) plang_parser_return_token(parser, maybe_array);
        goto bail_out;
    }

    plang_token_t lbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(lbracket, plang_token_type_LBRACKET)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_LBRACKET,
                              plang_parser_get_source(parser),
                              plang_token_get_range(lbracket));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(array_type);
    assert(node != NULL);

    plang_array_t index_types = plang_array_new(2);
    assert(index_types != NULL);

    node->_is_packed = is_packed;
    node->_index_types = index_types;

    bool done = false;
    do {
        const size_t index_type_start = plang_parser_get_position(parser);
        plang_node_t index_type = plang_node_index_type_parse(parser);
        if (index_type == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_index_type,
                                  plang_parser_get_source(parser),
                                  plang_range(index_type_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(index_types,
                                                   index_type);
        assert(appended != false);

        plang_token_t comma = plang_parser_next_significant_token(parser);
        if (plang_token_matches(comma, plang_token_type_COMMA)) {
            /* Continue looping. */
        } else {
            /* End loop */
            if (comma) plang_parser_return_token(parser, comma);
            done = true;
        }
    } while (!done);

    plang_token_t rbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(rbracket, plang_token_type_RBRACKET)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RBRACKET,
                              plang_parser_get_source(parser),
                              plang_token_get_range(rbracket));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_token_t of = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(of, plang_token_type_OF)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_OF,
                              plang_parser_get_source(parser),
                              plang_token_get_range(of));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t type_start = plang_parser_get_position(parser);
    plang_node_t type = plang_node_type_parse(parser);
    if (type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type,
                              plang_parser_get_source(parser),
                              plang_range(type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_type = type;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_array_type_free(struct plang_node_array_type *ns)
{
    plang_node_array_free(ns->_index_types);
    plang_node_free(ns->_type);
}

plang_node_t PLANG_NULLABLE
plang_node_index_type_parse(plang_parser_t parser)
{
    struct plang_node_index_type *node = NULL;

    plang_node_t ordinal_type = plang_node_ordinal_type_parse(parser);
    if (ordinal_type == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(index_type);
    assert(node);

    node->_ordinal_type = ordinal_type;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_index_type_free(struct plang_node_index_type *ns)
{
    plang_node_free(ns->_ordinal_type);
};

plang_node_t PLANG_NULLABLE
plang_node_set_type_parse(plang_parser_t parser,
                          bool is_packed)
{
    struct plang_node_set_type *node = NULL;

    plang_token_t maybe_set = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_set, plang_token_type_SET)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_set) plang_parser_return_token(parser, maybe_set);
        goto bail_out;
    }

    plang_token_t of = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(of, plang_token_type_OF)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_OF,
                              plang_parser_get_source(parser),
                              plang_token_get_range(of));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t ordinal_type_start = plang_parser_get_position(parser);
    plang_node_t ordinal_type = plang_node_ordinal_type_parse(parser);
    if (ordinal_type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_ordinal_type,
                              plang_parser_get_source(parser),
                              plang_range(ordinal_type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(set_type);
    assert(node != NULL);

    node->_ordinal_type = ordinal_type;
    node->_is_packed = is_packed;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_set_type_free(struct plang_node_set_type *ns)
{
    plang_node_free(ns->_ordinal_type);
}

/* No file type. */

plang_node_t PLANG_NULLABLE
plang_node_record_type_parse(plang_parser_t parser,
                             bool is_packed)
{
    struct plang_node_record_type *node = NULL;

    plang_token_t maybe_record = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_record, plang_token_type_RECORD)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_record) plang_parser_return_token(parser,
                                                    maybe_record);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(record_type);
    assert(node != NULL);

    plang_node_t field_list = plang_node_field_list_parse(parser);
    /* Optional */

    node->_field_list = field_list;

    plang_token_t end = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(end, plang_token_type_END)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_END,
                              plang_parser_get_source(parser),
                              plang_token_get_range(end));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_record_type_free(struct plang_node_record_type *ns)
{
    plang_node_free(ns->_field_list);
}

plang_node_t PLANG_NULLABLE
plang_node_field_list_parse(plang_parser_t parser)
{
    struct plang_node_field_list *node = NULL;

    plang_node_t fixed_part = plang_node_fixed_part_parse(parser);
    /* Optional */

    /*
     If there is a fixed-part, a semicolon is required before the
     introduction of a variant-part.

     Or, put another way: If there's a fixed-part, there must be a
     semicolon before introducing a variant-part. If there isn't a
     fixed-part, there must not be a semicolon before introducing a
     variant-part.
     */

    plang_token_t semicolon1 = NULL;
    plang_node_t variant_part = NULL;
    if (fixed_part) {
        semicolon1 = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(semicolon1,
                                 plang_token_type_SEMICOLON))
        {
            plang_parser_return_token(parser, semicolon1);
            semicolon1 = NULL;
        }
    }

    if (((fixed_part != NULL) && (semicolon1 != NULL)) ||
        ((fixed_part == NULL) && (semicolon1 == NULL)))
    {
        variant_part = plang_node_variant_parse(parser);
        /* Optional */
    }

    if (variant_part != NULL) {
        /*
         If there is a variant-part, there can be but doesn't have to be
         a semicolon to terminate it, at least according to the syntax
         diagrams in Section 3.2.2 Record-Types. This appears to be one
         of the only cases of an optional semicolon in the grammar.
         */

        plang_token_t semicolon2
            = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(semicolon2,
                                 plang_token_type_SEMICOLON))
        {
            plang_parser_return_token(parser, semicolon2);
        }
    }

    /*
     Only bother creating and returning the field-list if there would be
     something in it.
     */

    if ((fixed_part != NULL) || (variant_part != NULL)) {
        node = PLANG_NODE_NEW(field_list);
        assert(node != NULL);

        node->_fixed_part = fixed_part;
        node->_variant_part = variant_part;
    }

    return (plang_node_t) node;
}

void
plang_node_field_list_free(struct plang_node_field_list *ns)
{
    plang_node_free(ns->_fixed_part);
    plang_node_free(ns->_variant_part);
}

plang_node_t PLANG_NULLABLE
plang_node_field_declaration_parse(plang_parser_t parser)
{
    struct plang_node_field_declaration *node = NULL;

    plang_node_t identifier_list = plang_node_identifier_list_parse(parser);
    if (identifier_list == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(field_declaration);
    assert(node != NULL);

    node->_identifier_list = identifier_list;

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t type_start = plang_parser_get_position(parser);
    plang_node_t type = plang_node_type_parse(parser);
    if (type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type,
                              plang_parser_get_source(parser),
                              plang_range(type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_type = type;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_field_declaration_free(struct plang_node_field_declaration *ns)
{
    plang_node_free(ns->_identifier_list);
    plang_node_free(ns->_type);
}

plang_node_t PLANG_NULLABLE
plang_node_fixed_part_parse(plang_parser_t parser)
{
    struct plang_node_fixed_part *node = NULL;

    node = PLANG_NODE_NEW(fixed_part);
    assert(node != NULL);

    plang_array_t field_declarations = plang_array_new(4);
    assert(field_declarations != NULL);
    node->_field_declarations = field_declarations;

    bool first_time = true;
    bool done = false;
    do {
        const size_t field_declaration_start
            = plang_parser_get_position(parser);
        plang_node_t field_declaration
            = plang_node_field_declaration_parse(parser);
        if (field_declaration == NULL) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            } else {
                plang_error_t error
                    = plang_error_new(plang_error_type_expected_field_declaration,
                                      plang_parser_get_source(parser),
                                      plang_range(field_declaration_start, 0));
                plang_parser_signal_error(parser, error);
                goto bail_out;
            }
        }

        bool appended = plang_array_append(field_declarations,
                                           field_declaration);
        assert(appended != false);

        plang_token_t semicolon = plang_parser_next_significant_token(parser);
        if (plang_token_matches(semicolon,
                                plang_token_type_SEMICOLON)) {
            /* Keep iterating. */
        } else {
            /* Done iterating. */
            if (semicolon) plang_parser_return_token(parser, semicolon);
            done = true;
        }

        first_time = false;
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_fixed_part_free(struct plang_node_fixed_part *ns)
{
    plang_node_array_free(ns->_field_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_variant_part_parse(plang_parser_t parser)
{
    struct plang_node_variant_part *node = NULL;

    plang_token_t maybe_case = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_case, plang_token_type_CASE)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_case) plang_parser_return_token(parser, maybe_case);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(variant_part);
    assert(node != NULL);

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        if (identifier) plang_parser_return_token(parser, identifier);
        identifier = NULL;
    }
    node->_identifier = identifier;

    if (identifier) {
        plang_token_t colon = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(colon, plang_token_type_COLON)) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_COLON,
                                  plang_parser_get_source(parser),
                                  plang_token_get_range(colon));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
    }

    const size_t tag_field_type_start = plang_parser_get_position(parser);
    plang_node_t tag_field_type = plang_node_tag_field_type_parse(parser);
    if (tag_field_type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_tag_field_type,
                              plang_parser_get_source(parser),
                              plang_range(tag_field_type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_tag_field_type = tag_field_type;

    plang_token_t of = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(of, plang_token_type_OF)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_OF,
                              plang_parser_get_source(parser),
                              plang_token_get_range(of));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_array_t variants = plang_array_new(2);
    assert(variants != NULL);
    node->_variants = variants;

    bool done = false;
    do {
        const size_t variant_start = plang_parser_get_position(parser);
        plang_node_t variant = plang_node_variant_parse(parser);
        if (variant == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_variant,
                                  plang_parser_get_source(parser),
                                  plang_range(variant_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(variants, variant);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_SEMICOLON)) {
            /* Keep iterating. */
        } else {
            /*
             Done iterating. Put back the token since it may be the
             start of some other type.
             */
            if (token) plang_parser_return_token(parser, token);
            done = true;
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variant_part_free(struct plang_node_variant_part *ns)
{
    plang_node_free(ns->_tag_field_type);
    plang_node_array_free(ns->_variants);
}

/*!
 Parse a variant.

 Grammar:

     constant {',' constant} ':' '(' [field-list] ')'

 */
plang_node_t PLANG_NULLABLE
plang_node_variant_parse(plang_parser_t parser)
{
    struct plang_node_variant *node = NULL;

    node = PLANG_NODE_NEW(variant);
    assert(node != NULL);

    plang_array_t constants = plang_array_new(4);
    assert(constants != NULL);
    node->_constants = constants;

    bool first_time = true;
    bool done = false;
    do {
        const size_t constant_start = plang_parser_get_position(parser);
        plang_node_t constant = plang_node_constant_parse(parser);
        if (constant == NULL) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            } else {
                plang_error_t error
                    = plang_error_new(plang_error_type_expected_constant,
                                      plang_parser_get_source(parser),
                                      plang_range(constant_start, 0));
                plang_parser_signal_error(parser, error);
                goto bail_out;
            }
        }

        bool appended = plang_array_append(constants, constant);
        assert(appended != false);

    } while (!done);

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_token_t lparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(lparen, plang_token_type_LPAREN)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_LPAREN,
                              plang_parser_get_source(parser),
                              plang_token_get_range(lparen));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_node_t field_list = plang_node_field_list_parse(parser);
    /* Optional */
    node->_field_list = field_list;

    plang_token_t rparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(rparen, plang_token_type_RPAREN)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RPAREN,
                              plang_parser_get_source(parser),
                              plang_token_get_range(rparen));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variant_free(struct plang_node_variant *ns)
{
    plang_node_array_free(ns->_constants);
    plang_node_free(ns->_field_list);
}

plang_node_t PLANG_NULLABLE
plang_node_tag_field_type_parse(plang_parser_t parser)
{
    struct plang_node_tag_field_type *node = NULL;

    plang_node_t ordinal_type_identifier
        = plang_node_ordinal_type_identifier_parse(parser);
    if (ordinal_type_identifier == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(tag_field_type);
    node->_ordinal_type_identifier = ordinal_type_identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_tag_field_type_free(struct plang_node_tag_field_type *ns)
{
    plang_node_free(ns->_ordinal_type_identifier);
}

plang_node_t PLANG_NULLABLE
plang_node_pointer_type_parse(plang_parser_t parser)
{
    struct plang_node_pointer_type *node = NULL;

    node = (void *)plang_node_pointer_type_identifier_parse(parser);
    if (node) goto done;

    plang_token_t maybe_circumflex = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_circumflex,
                             plang_token_type_CIRCUMFLEX))
    {
        /* Just back out to backtrack. */
        if (maybe_circumflex) plang_parser_return_token(parser,
                                                        maybe_circumflex);
        goto bail_out;
    }

    const size_t type_identifier_start = plang_parser_get_position(parser);
    plang_node_t type_identifier = plang_node_type_identifier_parse(parser);
    if (type_identifier == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type_identifier,
                              plang_parser_get_source(parser),
                              plang_range(type_identifier_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(pointer_type);
    assert(node != NULL);

    node->_type_identifier = type_identifier;

done:
    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_pointer_type_free(struct plang_node_pointer_type *ns)
{
    plang_node_free(ns->_type_identifier);
}


/* MARK: - Variables */

plang_node_t PLANG_NULLABLE
plang_node_variable_declaration_parse(plang_parser_t parser)
{
    struct plang_node_variable_declaration *node = NULL;

    plang_node_t identifier_list = plang_node_identifier_list_parse(parser);
    if (identifier_list == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(variable_declaration);
    assert(node != NULL);

    node->_identifier_list = identifier_list;

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_source_t type_source = plang_parser_get_source(parser);
    const size_t type_start = plang_parser_get_position(parser);
    plang_node_t type = plang_node_type_parse(parser);
    if (type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type,
                              plang_parser_get_source(parser),
                              plang_range(type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    const size_t type_end = plang_parser_get_position(parser);
    node->_type = type;

    plang_token_t semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_scope_t scope = plang_parser_scope_current(parser);

    /*
     If the type isn't a type identifier, then its node needs to be
     registered with the current scope as an implicit, anonymous type
     declaration. A bogus identifier token will be synthesized for it
     (bogus in the sense that it doesn't obey the lexical rules for
     identifiers) and registering such a type should always succeed
     modulo allocation failure.
     */
    if (type->_type != plang_node_type_type_identifier) {
        plang_token_t variable_type_identifier
            = plang_token_new_anonymous_type_identifier(type_source,
                                                        plang_range(type_start,
                                                                    type_end));
        assert(variable_type_identifier != NULL);

        plang_type_t variable_type
            = plang_type_new(parser, variable_type_identifier, type);
        assert(variable_type != NULL);

        bool registered
            = plang_scope_type_register(scope, variable_type);
        assert(registered != false);
    }

    /*
     Register the variables with the current scope. Generate an error if
     the variable already exists within the current scope.
     */

    struct plang_node_identifier_list *variable_identifiers
        = (void *)identifier_list;
    const size_t count = plang_array_get_count(variable_identifiers->_identifiers);
    for (size_t i = 0; i < count; i++) {
        plang_token_t variable_identifier
            = plang_array_get_item(variable_identifiers->_identifiers, i);
        plang_variable_t variable = plang_variable_new(parser,
                                                       variable_identifier,
                                                       (plang_node_t) node);
        bool registered = plang_scope_variable_register(scope,
                                                        variable);
        if (registered == false) {
            plang_error_t error
                = plang_error_new(plang_error_type_already_exists_variable,
                                  plang_parser_get_source(parser),
                                  plang_token_get_range(variable_identifier));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variable_declaration_free(struct plang_node_variable_declaration *ns)
{
    plang_node_free(ns->_identifier_list);
    plang_node_free(ns->_type);
}

plang_node_t
plang_node_variable_declaration_get_type(plang_node_t node)
{
    assert(node->_type == plang_node_type_variable_declaration);

    struct plang_node_variable_declaration *ns = (void *) node;

    return ns->_type;
}

plang_node_t PLANG_NULLABLE
plang_node_variable_reference_parse(plang_parser_t parser)
{
    struct plang_node_variable_reference *node = NULL;

    plang_node_t variable_identifier
        = plang_node_variable_identifier_parse(parser);
    if (variable_identifier == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(variable_reference);
    assert(node != NULL);

    node->_variable_identifier = variable_identifier;

    plang_array_t qualifiers = plang_array_new(2);
    assert(qualifiers != NULL);
    node->_qualifiers = qualifiers;

    bool done = false;
    do {
        plang_node_t qualifier = plang_node_qualifier_parse(parser);
        if (qualifier == NULL) done = true;
        else {
            bool appended = plang_array_append(qualifiers, qualifier);
            assert(appended != false);
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variable_reference_free(struct plang_node_variable_reference *ns)
{
    plang_node_free(ns->_variable_identifier);
    plang_node_array_free(ns->_qualifiers);
}

plang_node_t
plang_node_variable_reference_get_type(plang_node_t node,
                                       plang_scope_t scope)
{
    plang_node_t type_node = NULL;

    assert(node->_type == plang_node_type_variable_reference);

    struct plang_node_variable_reference *ns = (void *)node;

    /*
     For a variable reference via a variable identifier, look up the
     variable in the given scope to find its declaration node. Then
     resolve the variable declaration node's type to a concrete type
     and return its node.
     */
    struct plang_node_variable_identifier *vis
        = (void *) ns->_variable_identifier;
    plang_token_t identifier = vis->_identifier;
    plang_variable_t variable
        = plang_scope_variable_lookup(scope, identifier, true);
    plang_type_t variable_type
        = plang_variable_get_type(variable, scope);
    type_node
        = plang_type_get_concrete_type_node(variable_type, scope);

    return type_node;
}


plang_node_t PLANG_NULLABLE
plang_node_variable_identifier_parse(plang_parser_t parser)
{
    struct plang_node_variable_identifier *node = NULL;

    plang_token_t identifier
        = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     Look up the variable in the current scope and all parent scopes.
     If no such variable exists, then back out.
     */

    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_variable_t variable = plang_scope_variable_lookup(scope,
                                                            identifier,
                                                            true);
    if (variable == NULL) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(variable_identifier);
    assert(node != NULL);

    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_variable_identifier_free(struct plang_node_variable_identifier *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_qualifier_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_index_parse(parser);
    if (node) return node;

    node = plang_node_field_designator_parse(parser);
    if (node) return node;

    /* No file type. */

    node = plang_node_pointer_object_symbol_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_index_parse(plang_parser_t parser)
{
    struct plang_node_index *node = NULL;

    plang_token_t maybe_lbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lbracket,
                             plang_token_type_LBRACKET)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_lbracket) plang_parser_return_token(parser,
                                                      maybe_lbracket);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(index);
    assert(node != NULL);

    plang_array_t expressions = plang_array_new(1);
    assert(expressions != NULL);
    node->_expressions = expressions;

    bool done = false;
    do {
        const size_t expression_start = plang_parser_get_position(parser);
        plang_node_t expression = plang_node_expression_parse(parser);
        if (expression == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_expression,
                                  plang_parser_get_source(parser),
                                  plang_range(expression_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        plang_array_append(expressions, expression);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_COMMA)) {
            /* Keep iterating. */
        } else if (plang_token_matches(token,
                                       plang_token_type_RBRACKET))
        {
            /* Done iterating. */
            done = true;
        } else {
            /*
             Put back token and keep iterating, since it may be the
             start of an expression.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_index_free(struct plang_node_index *ns)
{
    plang_node_array_free(ns->_expressions);
}

plang_node_t PLANG_NULLABLE
plang_node_field_designator_parse(plang_parser_t parser)
{
    struct plang_node_field_designator *node = NULL;

    plang_token_t maybe_period = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_period, plang_token_type_PERIOD)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_period) plang_parser_return_token(parser,
                                                    maybe_period);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(field_designator);
    assert(node != NULL);

    plang_token_t identifier
        = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_token,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_field_designator_free(struct plang_node_field_designator *ns)
{
    /* No sub-nodes. */
}

/* No file type. */

plang_node_t PLANG_NULLABLE
plang_node_pointer_object_symbol_parse(plang_parser_t parser)
{
    struct plang_node_pointer_object_symbol *node = NULL;

    plang_token_t maybe_circumflex = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_circumflex,
                             plang_token_type_CIRCUMFLEX))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_circumflex) plang_parser_return_token(parser,
                                                        maybe_circumflex);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(pointer_object_symbol);
    node->_circumflex = maybe_circumflex;

    /* Presence of node indicates pointer dereference. */

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_pointer_object_symbol_free(struct plang_node_pointer_object_symbol *ns)
{
    /* No sub-nodes. */
}


/* MARK: - Expressions */

plang_node_t PLANG_NULLABLE
plang_node_expression_parse(plang_parser_t parser)
{
    struct plang_node_expression *node = NULL;

    plang_node_t left_expression = plang_node_simple_expression_parse(parser);
    if (left_expression == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(expression);

    node->_left_expression = left_expression;

    plang_token_t maybe_operator = plang_parser_next_significant_token(parser);
    /* Optional */
    
    if (plang_token_is_expression_operator(maybe_operator)) {
        node->_operator = maybe_operator;

        const size_t right_expression_start = plang_parser_get_position(parser);
        plang_node_t right_expression = plang_node_simple_expression_parse(parser);
        if (right_expression == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_simple_expression,
                                  plang_parser_get_source(parser),
                                  plang_range(right_expression_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
        
        node->_right_expression = right_expression;
    } else {
        /* Just put the token back and be done. */
        if (maybe_operator) plang_parser_return_token(parser,
                                                      maybe_operator);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_expression_free(struct plang_node_expression *ns)
{
    plang_node_free(ns->_left_expression);
    plang_node_free(ns->_right_expression);
}

plang_node_t PLANG_NULLABLE
plang_node_factor_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_address_of_parse(parser);
    if (node) return node;

    node = plang_node_variable_reference_parse(parser);
    if (node) return node;

    node = plang_node_unsigned_constant_parse(parser);
    if (node) return node;

    node = plang_node_function_call_parse(parser);
    if (node) return node;

    node = plang_node_set_constructor_parse(parser);
    if (node) return node;

    node = plang_node_parenthesized_expression_parse(parser);
    if (node) return node;

    node = plang_node_antifactor_parse(parser);
    if (node) return node;

    return NULL;
}

plang_node_t PLANG_NULLABLE
plang_node_address_of_parse(plang_parser_t parser)
{
    struct plang_node_address_of *node = NULL;

    plang_token_t maybe_at = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_at, plang_token_type_AT)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_at) plang_parser_return_token(parser, maybe_at);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(address_of);
    assert(node != NULL);

    const size_t variable_reference_start
        = plang_parser_get_position(parser);
    plang_node_t variable_reference
        = plang_node_variable_reference_parse(parser);
    if (variable_reference == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_variable_reference,
                              plang_parser_get_source(parser),
                              plang_range(variable_reference_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_variable_reference = variable_reference;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_address_of_free(struct plang_node_address_of *ns)
{
    plang_node_free(ns->_variable_reference);
}

plang_node_t PLANG_NULLABLE
plang_node_unsigned_constant_parse(plang_parser_t parser)
{
    struct plang_node_unsigned_constant *node = NULL;
    
    node = PLANG_NODE_NEW(unsigned_constant);
    assert(node != NULL);

    plang_node_t unsigned_number
        = plang_node_unsigned_number_parse(parser);
    if (unsigned_number != NULL) {
        node->_unsigned_number = unsigned_number;
        goto done;
    }

    plang_token_t token = plang_parser_next_significant_token(parser);
    if (plang_token_matches(token, plang_token_type_string_literal)) {
        node->_quoted_string_constant = token;
        goto done;
    } else if (plang_token_matches(token, plang_token_type_NIL)) {
        node->_nil = token;
        goto done;
    } else {
        if (token) plang_parser_return_token(parser, token);

        /* See if it's an identifier of an unsigned constant. */

        plang_node_t constant_identifier
            = plang_node_constant_identifier_parse(parser);
        if (constant_identifier) {
            // TODO: Check constant_identifier is unsigned
            node->_constant_identifier = constant_identifier;
            goto done;
        }
    }

    /* Not what we're looking for, bail out to backtrack. */
    goto bail_out;

done:
    return (plang_node_t) node;
    
bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unsigned_constant_free(struct plang_node_unsigned_constant *ns)
{
    plang_node_free(ns->_unsigned_number);
    plang_node_free(ns->_constant_identifier);
}

plang_node_t PLANG_NULLABLE
plang_node_parenthesized_expression_parse(plang_parser_t parser)
{
    struct plang_node_parenthesized_expression *node = NULL;
    
    plang_token_t maybe_lparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lparen, plang_token_type_LPAREN)) {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, maybe_lparen);
        goto bail_out;
    }
    
    node = PLANG_NODE_NEW(parenthesized_expression);
    assert(node != NULL);

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_expression = expression;
    
    plang_token_t rparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(rparen, plang_token_type_RPAREN)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_RPAREN,
                              plang_parser_get_source(parser),
                              plang_token_get_range(rparen));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    
    return (plang_node_t) node;
    
bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_parenthesized_expression_free(struct plang_node_parenthesized_expression *ns)
{
    plang_node_free(ns->_expression);
}

plang_node_t PLANG_NULLABLE
plang_node_antifactor_parse(plang_parser_t parser)
{
    struct plang_node_antifactor *node = NULL;
    
    plang_token_t maybe_not = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_not, plang_token_type_NOT)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_not) plang_parser_return_token(parser, maybe_not);
        goto bail_out;
    }
    
    node = PLANG_NODE_NEW(antifactor);
    assert(node != NULL);

    const size_t factor_start = plang_parser_get_position(parser);
    plang_node_t factor = plang_node_factor_parse(parser);
    if (factor == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_factor,
                              plang_parser_get_source(parser),
                              plang_range(factor_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    
    return (plang_node_t) node;
    
bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_antifactor_free(struct plang_node_antifactor *ns)
{
    plang_node_free(ns->_factor);
}

plang_node_t PLANG_NULLABLE
plang_node_term_parse(plang_parser_t parser)
{
    struct plang_node_term *node = NULL;
    
    node = PLANG_NODE_NEW(term);
    assert(node != NULL);
    
    plang_array_t factors = plang_array_new(4);
    assert(factors != NULL);
    node->_factors = factors;
    
    plang_array_t operators = plang_array_new(4);
    assert(operators != NULL);
    node->_operators = operators;

    bool is_first = true;
    bool done = false;
    do {
        const size_t factor_start = plang_parser_get_position(parser);
        plang_node_t factor = plang_node_factor_parse(parser);
        if (factor == NULL) {
            if (is_first == false) {
                plang_error_t error
                    = plang_error_new(plang_error_type_expected_factor,
                                      plang_parser_get_source(parser),
                                      plang_range(factor_start, 0));
                plang_parser_signal_error(parser, error);
                goto bail_out;
            } else {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            }
        } else {
            is_first = false;
        }
        bool appended_factor = plang_array_append(factors,
                                                  factor);
        assert(appended_factor != false);
        
        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_is_factor_operator(token)) {
            /* Add and keep iterating. */
            bool appended_opertor = plang_array_append(operators,
                                                       token);
            assert(appended_opertor != false);
        } else {
            /* Put back and finish iterating. */
            if (token) plang_parser_return_token(parser, token);
            done = true;
        }
    } while (!done);
    
    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_term_free(struct plang_node_term *ns)
{
    plang_node_array_free(ns->_factors);
    plang_array_free(ns->_operators);
}

plang_node_t PLANG_NULLABLE
plang_node_simple_expression_parse(plang_parser_t parser)
{
    struct plang_node_simple_expression *node = NULL;

    node = PLANG_NODE_NEW(simple_expression);
    assert(node != NULL);

    node->_sign = plang_node_sign_parse(parser, true);
    /* Optional */

    plang_array_t terms = plang_array_new(4);
    assert(terms != NULL);
    node->_terms = terms;

    plang_array_t operators = plang_array_new(4);
    assert(operators != NULL);
    node->_operators = operators;

    bool done = false;
    do {
        plang_node_t term = plang_node_term_parse(parser);
        if (term == NULL) {
            /* Not what we're looking for, bail out to backtrack. */
            if (node->_sign) {
                plang_node_sign_unparse(parser, node->_sign);
            }
            goto bail_out;
        }

        bool appended_term = plang_array_append(terms, term);
        assert(appended_term != false);

        plang_token_t operator = plang_parser_next_significant_token(parser);
        if (plang_token_is_simple_expression_operator(operator)) {
            bool appended_operator = plang_array_append(operators,
                                                        operator);
            assert(appended_operator != false);

            /* Keep iterating. */
        } else {
            /* Return the token to the parser and stop iterating. */
            if (operator) plang_parser_return_token(parser, operator);
            done = true;
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_simple_expression_free(struct plang_node_simple_expression *ns)
{
    plang_node_free(ns->_sign);
    plang_node_array_free(ns->_terms);
    plang_array_free(ns->_operators);
}

plang_node_t PLANG_NULLABLE
plang_node_function_call_parse(plang_parser_t parser)
{
    struct plang_node_function_call *node = NULL;

    plang_node_t function_reference
        = plang_node_function_reference_parse(parser);
    if (function_reference == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_call);
    assert(node != NULL);

    node->_function_reference = function_reference;

    node->_actual_parameter_list
        = plang_node_actual_parameter_list_parse(parser);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_call_free(struct plang_node_function_call *ns)
{
    plang_node_free(ns->_function_reference);
    plang_node_free(ns->_actual_parameter_list);
}

plang_node_t PLANG_NULLABLE
plang_node_actual_parameter_list_parse(plang_parser_t parser)
{
    struct plang_node_actual_parameter_list *node = NULL;

    plang_token_t maybe_lparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lparen, plang_token_type_LPAREN)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_lparen) plang_parser_return_token(parser,
                                                    maybe_lparen);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(actual_parameter_list);
    assert(node != NULL);

    plang_array_t actual_parameters = plang_array_new(4);
    node->_actual_parameters = actual_parameters;

    bool done = false;
    do {
        const size_t actual_parameter_start
            = plang_parser_get_position(parser);
        plang_node_t actual_parameter
            = plang_node_actual_parameter_parse(parser);
        if (actual_parameter == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_actual_parameter,
                                  plang_parser_get_source(parser),
                                  plang_range(actual_parameter_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(actual_parameters,
                                           actual_parameter);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_COMMA)) {
            /* Consume token and keep iterating. */
        } else if (plang_token_matches(token, plang_token_type_RPAREN)) {
            /* Consume token and finish iterating. */
            done = true;
        } else {
            /*
             Return token and keep iterating, since it may start one of
             the syntax rules we care about.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_actual_parameter_list_free(struct plang_node_actual_parameter_list *ns)
{
    plang_node_array_free(ns->_actual_parameters);
}

plang_node_t PLANG_NULLABLE
plang_node_actual_parameter_parse(plang_parser_t parser)
{
    struct plang_node_actual_parameter *node = NULL;

    node = PLANG_NODE_NEW(actual_parameter);
    assert(node != NULL);

    const size_t actual_parameter_start
        = plang_parser_get_position(parser);
    plang_node_t subnode = plang_node_expression_parse(parser);
    if (subnode == NULL) {
        subnode = plang_node_variable_reference_parse(parser);
    }
    if (subnode == NULL) {
        subnode = plang_node_procedure_reference_parse(parser);
    }
    if (subnode == NULL) {
        subnode = plang_node_function_reference_parse(parser);
    }
    if (subnode == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_actual_parameter,
                              plang_parser_get_source(parser),
                              plang_range(actual_parameter_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_subnode = subnode;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_actual_parameter_free(struct plang_node_actual_parameter *ns)
{
    plang_node_free(ns->_subnode);
}

plang_node_t PLANG_NULLABLE
plang_node_set_constructor_parse(plang_parser_t parser)
{
    struct plang_node_set_constructor *node = NULL;
    
    plang_token_t maybe_lbracket = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lbracket,
                             plang_token_type_LBRACKET))
    {
        /* Not what we're looking for, bail out to backtrack. */
        plang_parser_return_token(parser, maybe_lbracket);
        goto bail_out;
    }
    
    node = PLANG_NODE_NEW(set_constructor);
    assert(node != NULL);
    
    plang_array_t member_groups = plang_array_new(4);
    assert(member_groups != NULL);
    
    node->_member_groups = member_groups;
    
    bool first_time = true;
    bool done = false;
    do {
        const size_t member_group_start = plang_parser_get_position(parser);
        plang_node_t member_group
            = plang_node_member_group_parse(parser);
        if (member_group == NULL) {
            if (first_time) {
                /* Just stop iterating, empty set. */
                done = true;
            } else {
                plang_error_t error
                    = plang_error_new(plang_error_type_expected_member_group,
                                      plang_parser_get_source(parser),
                                      plang_range(member_group_start, 0));
                plang_parser_signal_error(parser, error);
                goto bail_out;
            }
        } else {
            bool appended = plang_array_append(member_groups,
                                               member_group);
            assert(appended != false);

            plang_token_t token = plang_parser_next_significant_token(parser);
            if (plang_token_matches(token, plang_token_type_COMMA)) {
                /* Keep iterating. */
            } else if (plang_token_matches(token,
                                           plang_token_type_RBRACKET))
            {
                /* Stop iterating. */
                done = true;
            } else {
                /*
                 Put the token back and keep iterating because it may be
                 a token that starts a member-group.
                 */
                if (token) plang_parser_return_token(parser, token);
            }
        }
        if (first_time) first_time = false;
    } while (!done);
    
    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_set_constructor_free(struct plang_node_set_constructor *ns)
{
    plang_node_array_free(ns->_member_groups);
}

plang_node_t PLANG_NULLABLE
plang_node_member_group_parse(plang_parser_t parser)
{
    struct plang_node_member_group *node = NULL;

    plang_node_t start_expression = plang_node_expression_parse(parser);
    if (start_expression == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }
    
    node = PLANG_NODE_NEW(member_group);
    assert(node != NULL);
    
    node->_start_expression = start_expression;

    plang_token_t maybe_range = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_range, plang_token_type_RANGE)) {
        const size_t end_expression_start = plang_parser_get_position(parser);
        plang_node_t end_expression = plang_node_expression_parse(parser);
        if (end_expression == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_expression,
                                  plang_parser_get_source(parser),
                                  plang_range(end_expression_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
        node->_end_expression = end_expression;
    } else {
        if (maybe_range) plang_parser_return_token(parser, maybe_range);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_member_group_free(struct plang_node_member_group *ns)
{
    plang_node_free(ns->_start_expression);
    plang_node_free(ns->_end_expression);
}


/* MARK: - Statements */

plang_node_t PLANG_NULLABLE
plang_node_statement_parse(plang_parser_t parser)
{
    struct plang_node_statement *node = NULL;

    plang_node_t statement_label = plang_node_statement_label_parse(parser);
    /* Optional */

    node = PLANG_NODE_NEW(statement);
    assert(node != NULL);

    node->_statement_label = statement_label;

    plang_node_t statement = plang_node_simple_statement_parse(parser);
    if (statement == NULL) {
        statement = plang_node_structured_statement_parse(parser);
    }

    if (statement == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_statement_free(struct plang_node_statement *ns)
{
    plang_node_free(ns->_statement_label);
    plang_node_free(ns->_statement);
}

plang_node_t PLANG_NULLABLE
plang_node_statement_label_parse(plang_parser_t parser)
{
    struct plang_node_statement_label *node = NULL;

    plang_node_t maybe_label = plang_node_label_parse(parser);
    if (maybe_label == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(statement_label);
    assert(node != NULL);

    node->_label = maybe_label;

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_statement_label_free(struct plang_node_statement_label *ns)
{
    plang_node_free(ns->_label);
}

plang_node_t PLANG_NULLABLE
plang_node_simple_statement_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_assignment_statement_parse(parser);
    if (node) return node;

    node = plang_node_procedure_statement_parse(parser);
    if (node) return node;

    node = plang_node_goto_statement_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_assignment_statement_parse(plang_parser_t parser)
{
    struct plang_node_assignment_statement *node = NULL;

    plang_node_t variable_reference = NULL;
    plang_node_t function_identifier = NULL;

    variable_reference = plang_node_variable_reference_parse(parser);
    if (variable_reference == NULL) {
        function_identifier = plang_node_function_identifier_parse(parser);
    }

    if ((variable_reference == NULL) && (function_identifier == NULL)) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(assignment_statement);
    assert(node != NULL);

    node->_variable_reference = variable_reference;
    node->_function_identifier = function_identifier;

    plang_token_t assignment = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(assignment, plang_token_type_ASSIGNMENT)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_ASSIGNMENT,
                              plang_parser_get_source(parser),
                              plang_token_get_range(assignment));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_expression = expression;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_assignment_statement_free(struct plang_node_assignment_statement *ns)
{
    plang_node_free(ns->_variable_reference);
    plang_node_free(ns->_function_identifier);
    plang_node_free(ns->_expression);
}

plang_node_t PLANG_NULLABLE
plang_node_procedure_statement_parse(plang_parser_t parser)
{
    struct plang_node_procedure_statement *node = NULL;

    plang_node_t procedure_reference
        = plang_node_procedure_reference_parse(parser);
    if (procedure_reference == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    plang_node_t actual_parameter_list
        = plang_node_actual_parameter_list_parse(parser);

    node = PLANG_NODE_NEW(procedure_statement);
    assert(node != NULL);

    node->_procedure_reference = procedure_reference;
    node->_actual_parameter_list = actual_parameter_list;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_statement_free(struct plang_node_procedure_statement *ns)
{
    plang_node_free(ns->_procedure_reference);
    plang_node_free(ns->_actual_parameter_list);
}

plang_node_t PLANG_NULLABLE
plang_node_goto_statement_parse(plang_parser_t parser)
{
    struct plang_node_goto_statement *node = NULL;

    plang_token_t maybe_goto = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_goto, plang_token_type_GOTO)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_goto) plang_parser_return_token(parser, maybe_goto);
        goto bail_out;
    }

    const size_t label_start = plang_parser_get_position(parser);
    plang_node_t label = plang_node_label_parse(parser);
    if (label == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_label,
                              plang_parser_get_source(parser),
                              plang_range(label_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(goto_statement);
    node->_label = label;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_goto_statement_free(struct plang_node_goto_statement *ns)
{
    plang_node_free(ns->_label);
}

plang_node_t PLANG_NULLABLE
plang_node_structured_statement_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_compound_statement_parse(parser);
    if (node) return node;

    node = plang_node_conditional_statement_parse(parser);
    if (node) return node;

    node = plang_node_repetitive_statement_parse(parser);
    if (node) return node;

    node = plang_node_with_statement_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_compound_statement_parse(plang_parser_t parser)
{
    struct plang_node_compound_statement *node = NULL;

    plang_token_t maybe_begin = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_begin, plang_token_type_BEGIN)) {
        /* Just bail out, let caller signal error if appropriate. */
        if (maybe_begin) plang_parser_return_token(parser, maybe_begin);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(compound_statement);
    assert(node != NULL);

    plang_array_t statements = plang_array_new(8);
    assert(statements != NULL);

    node->_statements = statements;

    bool done = false;
    do {
        const size_t statement_start = plang_parser_get_position(parser);
        plang_node_t statement = plang_node_statement_parse(parser);
        if (statement == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_statement,
                                  plang_parser_get_source(parser),
                                  plang_range(statement_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(statements,
                                           statement);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_SEMICOLON)) {
            /* Consume the token and keep iterating. */
        } else if (plang_token_matches(token, plang_token_type_END)) {
            /* Consume the token and stop iterating. */
            done = true;
        } else {
            /* Return the token and keep iterating. */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_compound_statement_free(struct plang_node_compound_statement *ns)
{
    plang_array_free(ns->_statements);
}

plang_node_t PLANG_NULLABLE
plang_node_conditional_statement_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_if_statement_parse(parser);
    if (node) return node;

    node = plang_node_case_statement_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_if_statement_parse(plang_parser_t parser)
{
    struct plang_node_if_statement *node = NULL;

    plang_token_t maybe_if = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_if, plang_token_type_IF)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_if) plang_parser_return_token(parser, maybe_if);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(if_statement);
    assert(node != NULL);

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_expression = expression;

    plang_token_t then = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(then, plang_token_type_THEN)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_THEN,
                              plang_parser_get_source(parser),
                              plang_token_get_range(then));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t true_statement_start = plang_parser_get_position(parser);
    plang_node_t true_statement = plang_node_statement_parse(parser);
    if (true_statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(true_statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_true_statement = true_statement;

    plang_token_t maybe_else = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_else, plang_token_type_ELSE)) {
        const size_t false_statement_start
            = plang_parser_get_position(parser);
        plang_node_t false_statement = plang_node_statement_parse(parser);
        if (false_statement == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_statement,
                                  plang_parser_get_source(parser),
                                  plang_range(false_statement_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
        node->_false_statement = false_statement;
    } else {
        /* OK to just have IF/THEN */
        if (maybe_else) plang_parser_return_token(parser, maybe_else);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_if_statement_free(struct plang_node_if_statement *ns)
{
    plang_node_free(ns->_expression);
    plang_node_free(ns->_true_statement);
    plang_node_free(ns->_false_statement);
}


/*!
 Parse a case-statement

 Grammar:

     'case' expression
         'of' case-clause {',' case-clause}
         [otherwise-clause]
         [';'] 'end'
 */
plang_node_t PLANG_NULLABLE
plang_node_case_statement_parse(plang_parser_t parser)
{
    struct plang_node_case_statement *node = NULL;

    plang_token_t maybe_case = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_case, plang_token_type_CASE)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_case) plang_parser_return_token(parser, maybe_case);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(case_statement);
    assert(node != NULL);

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_expression = expression;

    plang_token_t of = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(of, plang_token_type_OF)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_OF,
                              plang_parser_get_source(parser),
                              plang_token_get_range(of));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_array_t cases = plang_array_new(4);
    assert(cases != NULL);
    node->_cases = cases;

    bool done = false;
    do {
        plang_node_t case_clause = plang_node_case_clause_parse(parser);
        if (case_clause == NULL) {
            /* Done iterating. */
            done = true;
        } else {
            bool appended = plang_array_append(cases, case_clause);
            assert(appended != false);

            plang_token_t token = plang_parser_next_significant_token(parser);
            if (plang_token_matches(token, plang_token_type_COMMA)) {
                /* Keep iterating. */
            } else {
                /*
                 Keep iterating but put back as it may represent the
                 start of a case-clause.
                 */
                if (token) plang_parser_return_token(parser, token);
            }
        }
    } while (!done);

    plang_node_t otherwise_clause = plang_node_otherwise_clause_parse(parser);
    /* Optional */
    node->_otherwise_clause = otherwise_clause;

    plang_token_t semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon, plang_token_type_SEMICOLON)) {
        /* Optional */
        if (semicolon) plang_parser_return_token(parser, semicolon);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_case_statement_free(struct plang_node_case_statement *ns)
{
    plang_node_free(ns->_expression);
    plang_node_array_free(ns->_cases);
    plang_node_free(ns->_otherwise_clause);
}

plang_node_t PLANG_NULLABLE
plang_node_case_clause_parse(plang_parser_t parser)
{
    struct plang_node_case_clause *node = NULL;

    node = PLANG_NODE_NEW(case_clause);
    assert(node != NULL);

    plang_array_t constants = plang_array_new(2);
    assert(constants != NULL);
    node->_constants = constants;

    bool first_time = true;
    bool done = false;
    do {
        const size_t constant_start = plang_parser_get_position(parser);
        plang_node_t constant = plang_node_constant_parse(parser);
        if (constant == NULL) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            } else {
                plang_error_t error
                    = plang_error_new(plang_error_type_expected_constant,
                                      plang_parser_get_source(parser),
                                      plang_range(constant_start, 0));
                plang_parser_signal_error(parser, error);
                goto bail_out;
            }
        }

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_COMMA)) {
            /* Keep iterating. */
        } else if (plang_token_matches(token, plang_token_type_COLON)) {
            /* Done iterating. */
            done = true;
        } else {
            /*
             Token may be starting a constant, so put it back and keep
             iterating.
             */
            if (token) plang_parser_return_token(parser, token);
        }

        first_time = false;
    } while (!done);

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t statement_start = plang_parser_get_position(parser);
    plang_node_t statement = plang_node_statement_parse(parser);
    if (statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_case_clause_free(struct plang_node_case_clause *ns)
{
    plang_node_array_free(ns->_constants);
    plang_node_free(ns->_statement);
}

plang_node_t PLANG_NULLABLE
plang_node_otherwise_clause_parse(plang_parser_t parser)
{
    struct plang_node_otherwise_clause *node = NULL;

    plang_token_t maybe_semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_semicolon,
                             plang_token_type_SEMICOLON))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_semicolon) plang_parser_return_token(parser,
                                                       maybe_semicolon);
        goto bail_out;
    }

    plang_token_t maybe_otherwise = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_otherwise,
                             plang_token_type_OTHERWISE))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_otherwise) plang_parser_return_token(parser,
                                                       maybe_otherwise);
        plang_parser_return_token(parser, maybe_semicolon);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(otherwise_clause);
    assert(node != NULL);

    const size_t statement_start = plang_parser_get_position(parser);
    plang_node_t statement = plang_node_statement_parse(parser);
    if (statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_otherwise_clause_free(struct plang_node_otherwise_clause *ns)
{
    plang_node_free(ns->_statement);
}

plang_node_t PLANG_NULLABLE
plang_node_repetitive_statement_parse(plang_parser_t parser)
{
    plang_node_t node = NULL;

    node = plang_node_repeat_statement_parse(parser);
    if (node) return node;

    node = plang_node_while_statement_parse(parser);
    if (node) return node;

    node = plang_node_for_statement_parse(parser);
    if (node) return node;

    return node;
}

plang_node_t PLANG_NULLABLE
plang_node_repeat_statement_parse(plang_parser_t parser)
{
    struct plang_node_repeat_statement *node = NULL;

    plang_token_t maybe_repeat = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_repeat, plang_token_type_REPEAT)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_repeat) plang_parser_return_token(parser,
                                                    maybe_repeat);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(repeat_statement);
    assert(node != NULL);

    plang_array_t statements = plang_array_new(4);
    assert(statements != NULL);
    node->_statements = statements;

    bool done = false;
    do {
        const size_t statement_start = plang_parser_get_position(parser);
        plang_node_t statement = plang_node_statement_parse(parser);
        if (statement == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_statement,
                                  plang_parser_get_source(parser),
                                  plang_range(statement_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(statements, statement);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_SEMICOLON)) {
            /* Keep iterating. */
        } else if (plang_token_matches(token, plang_token_type_UNTIL)) {
            /* Done iterating. */
            done = true;
        } else {
            /*
             Return the token and keep iterating since it may be the
             start of a statement.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_expression = expression;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_repeat_statement_free(struct plang_node_repeat_statement *ns)
{
    plang_node_free(ns->_expression);
    plang_node_array_free(ns->_statements);
}

plang_node_t PLANG_NULLABLE
plang_node_while_statement_parse(plang_parser_t parser)
{
    struct plang_node_while_statement *node = NULL;

    plang_token_t maybe_while = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_while, plang_token_type_WHILE)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_while) plang_parser_return_token(parser, maybe_while);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(while_statement);
    assert(node != NULL);

    const size_t expression_start = plang_parser_get_position(parser);
    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_expression,
                              plang_parser_get_source(parser),
                              plang_range(expression_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_expression = expression;

    plang_token_t maybe_do = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_do, plang_token_type_DO)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_DO,
                              plang_parser_get_source(parser),
                              plang_token_get_range(maybe_do));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t statement_start = plang_parser_get_position(parser);
    plang_node_t statement = plang_node_statement_parse(parser);
    if (statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_while_statement_free(struct plang_node_while_statement *ns)
{
    plang_node_free(ns->_expression);
    plang_node_free(ns->_statement);
}

plang_node_t PLANG_NULLABLE
plang_node_for_statement_parse(plang_parser_t parser)
{
    struct plang_node_for_statement *node = NULL;

    plang_token_t maybe_for = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_for, plang_token_type_FOR)) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(for_statement);
    assert(node != NULL);

    const size_t control_variable_start = plang_parser_get_position(parser);
    plang_node_t control_variable = plang_node_control_variable_parse(parser);
    if (control_variable == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_control_variable,
                              plang_parser_get_source(parser),
                              plang_range(control_variable_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_control_variable = control_variable;

    plang_token_t assignment = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(assignment, plang_token_type_ASSIGNMENT)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_ASSIGNMENT,
                              plang_parser_get_source(parser),
                              plang_token_get_range(assignment));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t initial_value_start = plang_parser_get_position(parser);
    plang_node_t initial_value = plang_node_initial_value_parse(parser);
    if (initial_value == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_initial_value_expression,
                              plang_parser_get_source(parser),
                              plang_range(initial_value_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_initial_value = initial_value;

    plang_token_t to_or_downto = plang_parser_next_significant_token(parser);
    if (plang_token_matches(to_or_downto, plang_token_type_TO)) {
        node->_counting_down = false;
    } else if (plang_token_matches(to_or_downto,
                                   plang_token_type_DOWNTO))
    {
        node->_counting_down = true;
    } else {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_token,
                              plang_parser_get_source(parser),
                              plang_token_get_range(to_or_downto));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t final_value_start = plang_parser_get_position(parser);
    plang_node_t final_value = plang_node_final_value_parse(parser);
    if (final_value == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_token,
                              plang_parser_get_source(parser),
                              plang_range(final_value_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_final_value = final_value;

    plang_token_t do_token = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(do_token, plang_token_type_DO)) {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_token,
                              plang_parser_get_source(parser),
                              plang_token_get_range(do_token));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t statement_start = plang_parser_get_position(parser);
    plang_node_t statement = plang_node_statement_parse(parser);
    if (statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_for_statement_free(struct plang_node_for_statement *ns)
{
    plang_node_free(ns->_control_variable);
    plang_node_free(ns->_initial_value);
    plang_node_free(ns->_final_value);
    plang_node_free(ns->_statement);
}

plang_node_t PLANG_NULLABLE
plang_node_control_variable_parse(plang_parser_t parser)
{
    struct plang_node_control_variable *node = NULL;

    const size_t variable_identifier_start = plang_parser_get_position(parser);
    plang_node_t variable_identifier = plang_node_variable_identifier_parse(parser);
    if (variable_identifier == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_variable_identifier,
                              plang_parser_get_source(parser),
                              plang_range(variable_identifier_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(control_variable);
    assert(node != NULL);

    node->_variable_identifier = variable_identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_control_variable_free(struct plang_node_control_variable *ns)
{
    plang_node_free(ns->_variable_identifier);
}

plang_node_t PLANG_NULLABLE
plang_node_initial_value_parse(plang_parser_t parser)
{
    struct plang_node_initial_value *node = NULL;

    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(initial_value);
    assert(node != NULL);

    node->_expression = expression;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_initial_value_free(struct plang_node_initial_value *ns)
{
    plang_node_free(ns->_expression);
}

plang_node_t PLANG_NULLABLE
plang_node_final_value_parse(plang_parser_t parser)
{
    struct plang_node_final_value *node = NULL;

    plang_node_t expression = plang_node_expression_parse(parser);
    if (expression == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(final_value);
    assert(node != NULL);

    node->_expression = expression;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_final_value_free(struct plang_node_final_value *ns)
{
    plang_node_free(ns->_expression);
}

/*!
 Typecheck a record variable reference.

 A `WITH` statement requires a reference to a record variable, so
 validate that the reference is to a variable that's known to be of
 record type.

 Errors are signaled by the caller (actually the caller's caller) so
 this can just return whether or not the check is successful.
 */
bool
plang_node_record_variable_reference_typecheck(plang_parser_t parser,
                                               struct plang_node_record_variable_reference * ns)
{
    /* Look up the variable itself. */

    struct plang_node_variable_reference *vrs
        = (void *)ns->_variable_reference;
    plang_node_t variable_identifier_node = vrs->_variable_identifier;
    struct plang_node_variable_identifier *vis
        = (void *)variable_identifier_node;
    plang_token_t variable_identifier = vis->_identifier;
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_variable_t variable
        = plang_scope_variable_lookup(scope, variable_identifier, true);
    if (variable == NULL) return false;

    /*
     Look up the variable's type and ensure the type is a record type.
     */

    plang_node_t variable_node = plang_variable_get_node(variable);
    struct plang_node_variable_declaration *vds = (void *)variable_node;
    plang_node_t variable_type = vds->_type;
    plang_node_t declaration
        = plang_node_type_get_type_declaration(variable_type);
    struct plang_node_type_declaration *tds = (void *)declaration;
    plang_token_t variable_type_identifier = tds->_identifier;
    assert(variable_type_identifier != NULL); /* should not happen */
    plang_type_t type
        = plang_scope_type_lookup(scope, variable_type_identifier, true);

    return plang_type_is_record(type, scope);
}

plang_node_t PLANG_NULLABLE
plang_node_record_variable_reference_parse(plang_parser_t parser)
{
    struct plang_node_record_variable_reference *node = NULL;

    plang_node_t variable_reference = plang_node_variable_reference_parse(parser);
    if (variable_reference == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(record_variable_reference);
    assert(node != NULL);

    node->_variable_reference = variable_reference;

    if (!plang_node_record_variable_reference_typecheck(parser, node)) {
        /* The error is signaled by the caller. */
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_record_variable_reference_free(struct plang_node_record_variable_reference *ns)
{
    plang_node_free(ns->_variable_reference);
}

plang_node_t PLANG_NULLABLE
plang_node_with_statement_parse(plang_parser_t parser)
{
    struct plang_node_with_statement *node = NULL;

    plang_token_t maybe_with = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_with, plang_token_type_WITH)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_with) plang_parser_return_token(parser, maybe_with);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(with_statement);
    assert(node != NULL);

    plang_array_t record_variable_references = plang_array_new(1);
    assert(record_variable_references != NULL);
    node->_record_variable_references = record_variable_references;

    bool done = false;
    do {
        const size_t record_variable_reference_start
            = plang_parser_get_position(parser);
        plang_node_t record_variable_reference
            = plang_node_record_variable_reference_parse(parser);
        if (record_variable_reference == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_record_variable_reference,
                                  plang_parser_get_source(parser),
                                  plang_range(record_variable_reference_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(record_variable_references,
                                           record_variable_reference);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_COMMA)) {
            /* Keep iterating. */
        } else if (plang_token_matches(token, plang_token_type_DO)) {
            /* Done iterating. */
            done = true;
        } else {
            /*
             Keep iterating after returning the token, since it may
             indicate the start of one of the rules we care about.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    const size_t statement_start = plang_parser_get_position(parser);
    plang_node_t statement = plang_node_statement_parse(parser);
    if (statement == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_statement,
                              plang_parser_get_source(parser),
                              plang_range(statement_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_statement = statement;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_with_statement_free(struct plang_node_with_statement *ns)
{
    plang_node_array_free(ns->_record_variable_references);
    plang_node_free(ns->_statement);
}


/* MARK: - Procedures & Functions */

plang_node_t PLANG_NULLABLE
plang_node_procedure_identifier_parse(plang_parser_t parser)
{
    struct plang_node_procedure_identifier *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }
    
    /*
     Look up the procedure in the current scope and all parent scopes.
     If no such procedure exists, then back out.
     */
    
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_procedure_t procedure = plang_scope_procedure_lookup(scope,
                                                               identifier,
                                                               true);
    if (procedure == NULL) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(procedure_identifier);
    assert(node != NULL);

    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_identifier_free(struct plang_node_procedure_identifier *ns)
{
    /* No sub-nodes. */
};

plang_node_t PLANG_NULLABLE
plang_node_function_identifier_parse(plang_parser_t parser)
{
    struct plang_node_function_identifier *node = NULL;

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (identifier) plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    /*
     Look up the function in the current scope and all parent scopes.
     If no such function exists, then back out.
     */
    
    plang_scope_t scope = plang_parser_scope_current(parser);
    plang_function_t function = plang_scope_function_lookup(scope,
                                                            identifier,
                                                            true);
    if (function == NULL) {
        plang_parser_return_token(parser, identifier);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_identifier);
    assert(node != NULL);

    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_identifier_free(struct plang_node_function_identifier *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_procedure_reference_parse(plang_parser_t parser)
{
    struct plang_node_procedure_reference *node = NULL;

    plang_node_t procedure_identifier
        = plang_node_procedure_identifier_parse(parser);
    if (procedure_identifier == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(procedure_reference);
    assert(node != NULL);

    node->_procedure_identifier = procedure_identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_reference_free(struct plang_node_procedure_reference *ns)
{
    plang_node_free(ns->_procedure_identifier);
}

plang_node_t PLANG_NULLABLE
plang_node_function_reference_parse(plang_parser_t parser)
{
    struct plang_node_function_reference *node = NULL;

    plang_node_t function_identifier
        = plang_node_function_identifier_parse(parser);
    if (function_identifier == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_reference);
    assert(node != NULL);

    node->_function_identifier = function_identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_reference_free(struct plang_node_function_reference *ns)
{
    plang_node_free(ns->_function_identifier);
}

plang_node_t PLANG_NULLABLE
plang_node_procedure_declaration_parse(plang_parser_t parser,
                                       bool allow_block)
{
    struct plang_node_procedure_declaration *node = NULL;

    plang_node_t procedure_heading = plang_node_procedure_heading_parse(parser);
    if (procedure_heading == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(procedure_declaration);
    assert(node != NULL);

    node->_procedure_heading = procedure_heading;

    plang_token_t semicolon1 = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon1, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon1));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_procedure_body = plang_node_procedure_body_parse(parser,
                                                            allow_block);
    if (node->_procedure_body != NULL) {
        /*
         Optional in an INTERFACE and enforced at the semantic layer
         rather than by the parser. The semicolon following any body is,
         however, not optional, and its presence is enforced by the
         parser.
         */
        plang_token_t semicolon2 = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(semicolon2, plang_token_type_SEMICOLON)) {
            plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon2));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
    }

    /*
     Pop the scope that was created for the procedure's formal
     parameters and block, so the next set of declarations or
     definitions are added to the next level of scope up.
     */

    (void) plang_parser_scope_pop(parser);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_declaration_free(struct plang_node_procedure_declaration *ns)
{
    plang_node_free(ns->_procedure_heading);
    plang_node_free(ns->_procedure_body);
}

/*!
 Parse a procedure heading and register it with the enclosing scope.

 Parses a procedure heading, looks it up in the local scope to ensure it
 isn't a duplicate, and registers it with the local scope. Also creates
 a scope to contain the procedure's formal parameter list and any body,
 and pushes it.

 - WARNING: If this function succeeds, the caller must arrange for the
            scope this function pushes to be popped at an appropriate
            point. (The scope will be popped automatically on failure.)
 */
plang_node_t PLANG_NULLABLE
plang_node_procedure_heading_parse(plang_parser_t parser)
{
    struct plang_node_procedure_heading *node = NULL;

    plang_token_t maybe_procedure = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_procedure,
                             plang_token_type_PROCEDURE))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_procedure) plang_parser_return_token(parser,
                                                       maybe_procedure);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(procedure_heading);
    assert(node != NULL);

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_identifier = identifier;

    /*
     Register the procedure with the current scope; generate an error if
     the procedure already exists within the current scope.

     This needs to be done before parsing the body, because the body may
     contain recursion.

     It's OK to continue filling in the procedure's node after it's
     registered with the scope.
     */

    plang_scope_t current_scope = plang_parser_scope_current(parser);
    plang_procedure_t procedure = plang_procedure_new(parser,
                                                      identifier,
                                                      (plang_node_t) node);

    bool registered = plang_scope_procedure_register(current_scope,
                                                     procedure);
    if (registered == false) {
        plang_error_t error
            = plang_error_new(plang_error_type_already_exists_procedure,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /*
     Create a scope for the procedure, covering both its formal
     parameter list and its block (if any).
     */

    plang_scope_t local_scope = plang_scope_new(current_scope);
    assert(local_scope != NULL);

    node->_scope = local_scope;

    bool pushed = plang_parser_scope_push(parser, local_scope);
    assert(pushed != false);

    plang_node_t formal_parameter_list
        = plang_node_formal_parameter_list_parse(parser);
    /* Optional, OK if it's NULL */

    node->_formal_parameter_list = formal_parameter_list;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_heading_free(struct plang_node_procedure_heading *ns)
{
    plang_scope_free(ns->_scope);

    plang_node_free(ns->_formal_parameter_list);
}

plang_node_t PLANG_NULLABLE
plang_node_procedure_body_parse(plang_parser_t parser,
                                bool allow_block)
{
    struct plang_node_procedure_body *node = NULL;

    bool is_forward = false;
    bool is_external = false;

    /*
     Did we get an identifier matching FORWARD or EXTERNAL? Those are
     declarations, not reserved words, so we can't rely on the reserved
     word parser.
     */
    plang_token_t maybe_identifier
        = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_identifier,
                            plang_token_type_identifier))
    {
        /*
         If there was an identifier, see if it indicates a FORWARD or
         EXTERNAL declaration for the procedure.
         */

        is_forward = plang_token_identifier_equals(maybe_identifier,
                                                   "FORWARD");

        is_external = (!is_forward &&
                       plang_token_identifier_equals(maybe_identifier,
                                                     "EXTERNAL"));

        /*
         If there was an identifier but it's not FORWARD or EXTERNAL,
         then it might be part of a block, so just return it to the
         parser for trying with that.
         */
        if (!is_forward && !is_external) {
            if (maybe_identifier)
                plang_parser_return_token(parser, maybe_identifier);
        }
    } else {
        /*
         If there was a non-identifier then it might be part of the next
         PROCEDURE or FUNCTION or might indicate the start of a block,
         so just return it to the parser for trying with that.
         */
        if (maybe_identifier)
            plang_parser_return_token(parser, maybe_identifier);
    }

    /* INTERFACE doesn't allow a block. */
    plang_node_t block = NULL;
    if (!is_forward && !is_external && allow_block) {
        block = plang_node_block_parse(parser);
        /*
         Optional for procedure declarations in an INTERFACE, enforced
         at the semantic layer rather than by the parser.
         */
    }

    if (!is_forward && !is_external && (block == NULL)) {
        /*
         If there are no declarations and there is no block, don't
         bother instantiating a procedure body.
         */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(procedure_body);
    assert(node != NULL);

    node->_block = block;
    node->_is_forward = is_forward;
    node->_is_external = is_external;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_procedure_body_free(struct plang_node_procedure_body *ns)
{
    plang_node_free(ns->_block);
}

plang_scope_t
plang_node_procedure_get_scope(plang_node_t node)
{
    switch (node->_type) {
        case plang_node_type_procedure_declaration: {
            struct plang_node_procedure_declaration *nspd = (void *)node;
            plang_node_t heading = nspd->_procedure_heading;
            struct plang_node_procedure_heading *nsph = (void *)heading;
            return nsph->_scope;
        }

        case plang_node_type_procedure_heading: {
            struct plang_node_procedure_heading *nsfh = (void *)node;
            return nsfh->_scope;
        }

        default:
            assert(false); /* Should never get here. */
    }
}

plang_token_t
plang_node_function_heading_get_identifier(plang_node_t heading_node)
{
    struct plang_node_function_heading *heading = (void *)heading_node;
    
    return heading->_identifier;
}

plang_node_t PLANG_NULLABLE
plang_node_function_declaration_parse(plang_parser_t parser,
                                      bool allow_block)
{
    struct plang_node_function_declaration *node = NULL;

    plang_node_t function_heading = plang_node_function_heading_parse(parser);
    if (function_heading == NULL) {
        /* Not what we're looking for, bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_declaration);
    assert(node != NULL);

    node->_function_heading = function_heading;

    plang_token_t semicolon1 = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon1, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon1));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_function_body = plang_node_function_body_parse(parser,
                                                          allow_block);
    if (node->_function_body != NULL) {
        /*
         Optional in an INTERFACE and enforced at the semantic layer
         rather than by the parser. The semicolon following any body is,
         however, not optional, and its presence is enforced by the
         parser.
         */
        plang_token_t semicolon2 = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(semicolon2, plang_token_type_SEMICOLON)) {
            plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon2));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
    }

    /*
     Pop the scope that was created for the procedure's formal
     parameters and block, so the next set of declarations or
     definitions are added to the next level of scope up.
     */

    (void) plang_parser_scope_pop(parser);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_declaration_free(struct plang_node_function_declaration *ns)
{
    plang_node_free(ns->_function_heading);
    plang_node_free(ns->_function_body);
}

/*!
 Parse a function heading and register it with the enclosing scope.

 Parses a function heading, looks it up in the local scope to ensure it
 isn't a duplicate, and registers it with the local scope. Also creates
 a scope to contain the function's formal parameter list and any body,
 and pushes it.

 - WARNING: If this function succeeds, the caller must arrange for the
            scope this function pushes to be popped at an appropriate
            point. (The scope will be popped automatically on failure.)
 */
plang_node_t PLANG_NULLABLE
plang_node_function_heading_parse(plang_parser_t parser)
{
    struct plang_node_function_heading *node = NULL;

    plang_token_t maybe_function = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_function,
                             plang_token_type_FUNCTION))
    {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_function) plang_parser_return_token(parser,
                                                      maybe_function);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_heading);
    assert(node != NULL);

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_identifier = identifier;

    /*
     Register the function with the current scope; generate an error if
     the function already exists within the current scope.

     This needs to be done before parsing the body, because the body may
     contain recursion or statements assigning a result to the function.

     It's OK to continue filling in the function's node after it's
     registered with the scope.
     */

    plang_scope_t current_scope = plang_parser_scope_current(parser);
    plang_function_t function = plang_function_new(parser,
                                                   identifier,
                                                   (plang_node_t) node);

    bool registered = plang_scope_function_register(current_scope,
                                                    function);
    if (registered == false) {
        plang_error_t error
            = plang_error_new(plang_error_type_already_exists_function,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /*
     Create a scope for the function, covering both its formal
     parameter list and its block (if any).
     */

    plang_scope_t local_scope = plang_scope_new(current_scope);
    assert(local_scope != NULL);

    node->_scope = local_scope;

    bool pushed = plang_parser_scope_push(parser, local_scope);
    assert(pushed != false);

    plang_node_t formal_parameter_list
        = plang_node_formal_parameter_list_parse(parser);
    /* Optional, OK if it's NULL */

    node->_formal_parameter_list = formal_parameter_list;

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t result_type_start = plang_parser_get_position(parser);
    plang_node_t result_type = plang_node_result_type_parse(parser);
    if (result_type == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_result_type,
                              plang_parser_get_source(parser),
                              plang_range(result_type_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_result_type = result_type;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_heading_free(struct plang_node_function_heading *ns)
{
    plang_scope_free(ns->_scope);

    plang_node_free(ns->_formal_parameter_list);
    plang_node_free(ns->_result_type);
}

plang_node_t
plang_node_function_heading_get_result_type(plang_node_t node)
{
    assert(node->_type == plang_node_type_function_heading);

    struct plang_node_function_heading *ns = (void *) node;

    return ns->_result_type;
}

plang_node_t PLANG_NULLABLE
plang_node_result_type_parse(plang_parser_t parser)
{
    struct plang_node_result_type *node = NULL;

    node = PLANG_NODE_NEW(result_type);
    assert(node != NULL);

    const size_t type_identifier_start = plang_parser_get_position(parser);
    plang_node_t type_identifier = NULL;
    type_identifier = plang_node_ordinal_type_identifier_parse(parser);
    node->_ordinal_type_identifier = type_identifier;
    if (type_identifier == NULL) {
        type_identifier = plang_node_real_type_identifier_parse(parser);
        node->_real_type_identifier = type_identifier;
    }
    if (type_identifier == NULL) {
        type_identifier
            = plang_node_pointer_type_identifier_parse(parser);
        node->_pointer_type_identifier = type_identifier;
    }

    if (type_identifier == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type_identifier,
                              plang_parser_get_source(parser),
                              plang_range(type_identifier_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_result_type_free(struct plang_node_result_type *ns)
{
    plang_node_free(ns->_ordinal_type_identifier);
    plang_node_free(ns->_real_type_identifier);
    plang_node_free(ns->_pointer_type_identifier);
}

plang_node_t
plang_node_result_type_get_type(plang_node_t node)
{
    assert(node->_type == plang_node_type_result_type);

    struct plang_node_result_type *ns = (void *) node;

    if (ns->_ordinal_type_identifier)
        return ns->_ordinal_type_identifier;
    if (ns->_real_type_identifier)
        return ns->_real_type_identifier;
    if (ns->_pointer_type_identifier)
        return ns->_pointer_type_identifier;

    assert(false);
}

plang_node_t PLANG_NULLABLE
plang_node_function_body_parse(plang_parser_t parser,
                               bool allow_body)
{
    struct plang_node_function_body *node = NULL;

    bool is_forward = false;
    bool is_external = false;

    /*
     Did we get an identifier matching FORWARD or EXTERNAL? Those are
     declarations, not reserved words, so we can't rely on the reserved
     word parser.
     */
    plang_token_t maybe_identifier
        = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_identifier,
                            plang_token_type_identifier))
    {
        /*
         If there was an identifier, see if it indicates a FORWARD or
         EXTERNAL declaration for the procedure.
         */

        is_forward = plang_token_identifier_equals(maybe_identifier,
                                                   "FORWARD");

        is_external = (!is_forward &&
                       plang_token_identifier_equals(maybe_identifier,
                                                     "EXTERNAL"));

        /*
         If there was an identifier but it's not FORWARD or EXTERNAL,
         then it might be part of a block, so just return it to the
         parser for trying with that.
         */
        if (!is_forward && !is_external) {
            if (maybe_identifier)
                plang_parser_return_token(parser, maybe_identifier);
        }
    } else {
        /*
         If there was a non-identifier then it might be part of the next
         PROCEDURE or FUNCTION or might indicate the start of a block,
         so just return it to the parser for trying with that.
         */
        if (maybe_identifier)
            plang_parser_return_token(parser, maybe_identifier);
    }

    /* IMPLEMENTATION does allow a block */
    plang_node_t block = NULL;
    if (!is_forward && !is_external && allow_body) {
        block = plang_node_block_parse(parser);
        /*
         Optional for procedure declarations in an INTERFACE, enforced
         at the semantic layer rather than by the parser.
         */
    }

    if (!is_forward && !is_external && (block == NULL)) {
        /*
         If there are no declarations and there is no body, don't bother
         instantiating a function body.
         */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(function_body);
    assert(node != NULL);

    node->_block = block;
    node->_is_forward = is_forward;
    node->_is_external = is_external;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_function_body_free(struct plang_node_function_body *ns)
{
    plang_node_free(ns->_block);
}

plang_scope_t
plang_node_function_get_scope(plang_node_t node)
{
    switch (node->_type) {
        case plang_node_type_function_declaration: {
            struct plang_node_function_declaration *nsfd = (void *)node;
            plang_node_t heading = nsfd->_function_heading;
            struct plang_node_function_heading *nsfh = (void *)heading;
            return nsfh->_scope;
        }

        case plang_node_type_function_heading: {
            struct plang_node_function_heading *nsfh = (void *)node;
            return nsfh->_scope;
        }

        default:
            assert(false); /* Should never get here. */
    }
}

plang_node_t PLANG_NULLABLE
plang_node_formal_parameter_list_parse(plang_parser_t parser)
{
    struct plang_node_formal_parameter_list *node = NULL;

    plang_token_t maybe_lparen = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_lparen, plang_token_type_LPAREN)) {
        /* Not what we're looking for, bail out to backtrack. */
        if (maybe_lparen)
            plang_parser_return_token(parser, maybe_lparen);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(formal_parameter_list);
    assert(node != NULL);

    plang_array_t parameter_declarations = plang_array_new(4);
    assert(parameter_declarations != NULL);

    node->_parameter_declarations = parameter_declarations;

    bool done = false;
    do {
        const size_t item_start = plang_parser_get_position(parser);
        plang_node_t item = plang_node_parameter_declaration_parse(parser);

        if (item == NULL) {
            item = plang_node_procedure_heading_parse(parser);
            if (item != NULL) (void) plang_parser_scope_pop(parser);
        }

        if (item == NULL) {
            item = plang_node_function_heading_parse(parser);
            if (item != NULL) (void) plang_parser_scope_pop(parser);
        }

        if (item == NULL) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_parameter_declaration,
                                  plang_parser_get_source(parser),
                                  plang_range(item_start, 0));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }

        bool appended = plang_array_append(parameter_declarations,
                                           item);
        assert(appended != false);

        plang_token_t token = plang_parser_next_significant_token(parser);
        if (plang_token_matches(token, plang_token_type_SEMICOLON)) {
            /* Keep iterating */
        } else if (plang_token_matches(token,
                                       plang_token_type_RPAREN))
        {
            /* Finished iterating */
            done = true;
        } else {
            /*
             Keep iterating, after returning the token because it may be
             starting one of the rules we care about.
             */
            if (token) plang_parser_return_token(parser, token);
        }
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_formal_parameter_list_free(struct plang_node_formal_parameter_list *ns)
{
    plang_node_array_free(ns->_parameter_declarations);
}

plang_node_t PLANG_NULLABLE
plang_node_parameter_declaration_parse(plang_parser_t parser)
{
    struct plang_node_parameter_declaration *node = NULL;

    bool is_var = false;
    plang_token_t maybe_var = plang_parser_next_significant_token(parser);
    if (plang_token_matches(maybe_var, plang_token_type_VAR)) {
        is_var = true;
    } else {
        if (maybe_var) plang_parser_return_token(parser, maybe_var);
    }

    node = PLANG_NODE_NEW(parameter_declaration);
    assert(node != NULL);

    node->_is_var = is_var;

    const size_t identifier_list_start = plang_parser_get_position(parser);
    plang_node_t identifier_list = plang_node_identifier_list_parse(parser);
    if (identifier_list == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier_list,
                              plang_parser_get_source(parser),
                              plang_range(identifier_list_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_identifier_list = identifier_list;

    plang_token_t colon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(colon, plang_token_type_COLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_COLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(colon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    const size_t type_identifier_start = plang_parser_get_position(parser);
    plang_node_t type_identifier = plang_node_type_identifier_parse(parser);
    if (type_identifier == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_type_identifier,
                              plang_parser_get_source(parser),
                              plang_range(type_identifier_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    node->_type_identifier = type_identifier;

    /*
     Register the parameter declarations with the current scope as if
     they were variable declarations.
     */

    plang_scope_t scope = plang_parser_scope_current(parser);
    struct plang_node_identifier_list *idlist = (void *)identifier_list;
    plang_array_t ids = idlist->_identifiers;
    const size_t count = plang_array_get_count(ids);
    for (size_t idx = 0; idx < count; idx++) {
        plang_token_t id = plang_array_get_item(ids, idx);
        plang_variable_t variable
            = plang_variable_new(parser, id, (plang_node_t) node);
        assert(variable != NULL);

        bool registered = plang_scope_variable_register(scope,
                                                        variable);
        assert(registered != false);
    }

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_parameter_declaration_free(struct plang_node_parameter_declaration *ns)
{
    plang_node_free(ns->_identifier_list);
    plang_node_free(ns->_type_identifier);
}

plang_node_t
plang_node_parameter_declaration_get_type_identifier(plang_node_t node)
{
    assert(node->_type == plang_node_type_parameter_declaration);

    struct plang_node_parameter_declaration *ns = (void *) node;

    return ns->_type_identifier;
}


/* MARK: - Programs */

plang_node_t PLANG_NULLABLE
plang_node_program_parse(plang_parser_t parser)
{
    struct plang_node_program *node = NULL;
    bool pushed_program_scope = false;

    plang_node_t program_heading = plang_node_program_heading_parse(parser);
    if (program_heading == NULL) {
        /* Not actually an error, just back out of this parse. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(program);
    assert(node != NULL);

    node->_program_heading = program_heading;

    plang_token_t semicolon1 = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon1, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon1));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /* Every program has an associated scope. */

    plang_scope_t current_scope = plang_parser_scope_current(parser);

    plang_scope_t program_scope = plang_scope_new(current_scope);
    assert(program_scope != NULL);

    pushed_program_scope = plang_parser_scope_push(parser,
                                                   program_scope);
    assert(pushed_program_scope != false);

    node->_scope = program_scope;

    node->_uses_clause = plang_node_uses_clause_parse(parser);
    /* Optional, but if present must be terminated with semicolon. */

    if (node->_uses_clause) {
        plang_token_t semicolon2 = plang_parser_next_significant_token(parser);
        if (!plang_token_matches(semicolon2, plang_token_type_SEMICOLON)) {
            plang_error_t error
                = plang_error_new(plang_error_type_expected_SEMICOLON,
                                  plang_parser_get_source(parser),
                                  plang_token_get_range(semicolon2));
            plang_parser_signal_error(parser, error);
            goto bail_out;
        }
    }

    const size_t block_start = plang_parser_get_position(parser);
    node->_block = plang_node_block_parse(parser);
    if (node->_block == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_block,
                              plang_parser_get_source(parser),
                              plang_range(block_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_token_t period = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(period, plang_token_type_PERIOD)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_PERIOD,
                              plang_parser_get_source(parser),
                              plang_token_get_range(period));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /* Pop the scope once we're done parsing the program. */

    (void) plang_parser_scope_pop(parser);

    return (plang_node_t) node;

bail_out:
    /* Be sure to pop scope if exiting due to an error. */
    if (pushed_program_scope) {
        (void) plang_parser_scope_pop(parser);
    }

    plang_node_free((plang_node_t)node);
    return NULL;
}

void
plang_node_program_free(struct plang_node_program *ns)
{
    plang_scope_free(ns->_scope);

    plang_node_free(ns->_program_heading);
    plang_node_free(ns->_uses_clause);
    plang_node_free(ns->_block);
}

plang_token_t
plang_node_program_get_identifier(plang_node_t node)
{
    assert(node->_type == plang_node_type_program);

    struct plang_node_program *p = (void *)node;
    struct plang_node_program_heading *ph = (void *)p->_program_heading;
    return ph->_identifier;
}

plang_scope_t
plang_node_program_get_scope(plang_node_t node)
{
    assert(node->_type == plang_node_type_program);

    return ((struct plang_node_program *) node)->_scope;
}

plang_node_t PLANG_NULLABLE
plang_node_program_heading_parse(plang_parser_t parser)
{
    struct plang_node_program_heading *node = NULL;

    plang_token_t maybe_program = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_program, plang_token_type_PROGRAM)) {
        /* Just bail out to backtrack. */
        if (maybe_program) plang_parser_return_token(parser,
                                                     maybe_program);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(program_heading);
    assert(node != NULL);

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_program_heading_free(struct plang_node_program_heading *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_uses_clause_parse(plang_parser_t parser)
{
    struct plang_node_uses_clause *node = NULL;

    plang_token_t maybe_uses = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_uses, plang_token_type_USES)) {
        /* Bail out to backtrack. */
        if (maybe_uses) plang_parser_return_token(parser, maybe_uses);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(uses_clause);
    assert(node != NULL);

    const size_t identifier_list_start = plang_parser_get_position(parser);
    plang_node_t identifier_list = plang_node_identifier_list_parse(parser);
    if (identifier_list == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier_list,
                              plang_parser_get_source(parser),
                              plang_range(identifier_list_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_identifier_list = identifier_list;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_uses_clause_free(struct plang_node_uses_clause *ns)
{
    plang_node_free(ns->_identifier_list);
}


/* MARK: - Units */

plang_node_t PLANG_NULLABLE
plang_node_unit_parse(plang_parser_t parser)
{
    struct plang_node_unit *node = NULL;
    bool pushed_interface_scope = false;
    bool pushed_implementation_scope = false;

    plang_node_t unit_heading = plang_node_unit_heading_parse(parser);
    if (unit_heading == NULL) {
        /* Bail out to backtrack. */
        goto bail_out;
    }

    node = PLANG_NODE_NEW(unit);
    assert(node != NULL);

    node->_unit_heading = unit_heading;

    plang_token_t semicolon = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(semicolon, plang_token_type_SEMICOLON)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_SEMICOLON,
                              plang_parser_get_source(parser),
                              plang_token_get_range(semicolon));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /*
     Every interface-part and implementation-part has an associated
     scope.
     */

    plang_scope_t current_scope = plang_parser_scope_current(parser);

    plang_scope_t interface_scope = plang_scope_new(current_scope);
    assert(interface_scope != NULL);

    pushed_interface_scope
        = plang_parser_scope_push(parser, interface_scope);
    assert(pushed_interface_scope != false);

    node->_interface_scope = interface_scope;

    const size_t interface_part_start = plang_parser_get_position(parser);
    plang_node_t interface_part = plang_node_interface_part_parse(parser);
    if (interface_part == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_interface_part,
                              plang_parser_get_source(parser),
                              plang_range(interface_part_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_interface_part = interface_part;

    /*
     The implementation-part's scope is nesteed within the scope of the
     interface-part, so lookups of declarations will succeed and can be
     shadowed by definitions.
     */

    plang_scope_t implementation_scope = plang_scope_new(interface_scope);
    assert(implementation_scope != NULL);

    pushed_implementation_scope
        = plang_parser_scope_push(parser, implementation_scope);
    assert(pushed_implementation_scope != false);

    node->_implementation_scope = implementation_scope;

    const size_t implementation_part_start = plang_parser_get_position(parser);
    plang_node_t implementation_part = plang_node_implementation_part_parse(parser);
    if (implementation_part == NULL) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_implementation_part,
                              plang_parser_get_source(parser),
                              plang_range(implementation_part_start, 0));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_implementation_part = implementation_part;

    plang_token_t end = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(end, plang_token_type_END)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_END,
                              plang_parser_get_source(parser),
                              plang_token_get_range(end));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    plang_token_t period = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(period, plang_token_type_PERIOD)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_PERIOD,
                              plang_parser_get_source(parser),
                              plang_token_get_range(period));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }

    /*
     Pop scopes once the unit is fully parsed. Scopes for units used in
     a program or unit will be pushed as appropriate via other means.
     */

    if (pushed_implementation_scope) {
        (void) plang_parser_scope_pop(parser);
    }

    if (pushed_interface_scope) {
        (void) plang_parser_scope_pop(parser);
    }

    return (plang_node_t) node;

bail_out:
    /* Be sure to pop scopes if exiting due to an error. */
    if (pushed_implementation_scope) {
        (void) plang_parser_scope_pop(parser);
    }

    if (pushed_interface_scope) {
        (void) plang_parser_scope_pop(parser);
    }

    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unit_free(struct plang_node_unit *ns)
{
    plang_scope_free(ns->_interface_scope);
    plang_scope_free(ns->_implementation_scope);

    plang_node_free(ns->_unit_heading);
    plang_node_free(ns->_interface_part);
    plang_node_free(ns->_implementation_part);
}

plang_token_t
plang_node_unit_get_identifier(plang_node_t node)
{
    assert(node->_type == plang_node_type_unit);

    struct plang_node_unit *u = (void *)node;
    struct plang_node_unit_heading *uh = (void *)u->_unit_heading;
    return uh->_identifier;
}

plang_scope_t
plang_node_unit_get_interface_scope(plang_node_t node)
{
    assert(node->_type == plang_node_type_unit);

    struct plang_node_unit *u = (void *)node;
    return u->_interface_scope;
}

plang_scope_t
plang_node_unit_get_implementation_scope(plang_node_t node)
{
    assert(node->_type == plang_node_type_unit);

    struct plang_node_unit *u = (void *)node;
    return u->_implementation_scope;
}

plang_node_t PLANG_NULLABLE
plang_node_unit_heading_parse(plang_parser_t parser)
{
    struct plang_node_unit_heading *node = NULL;

    plang_token_t maybe_unit_token = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_unit_token, plang_token_type_UNIT)) {
        /* Just bail out to backtrack. */
        if (maybe_unit_token)
            plang_parser_return_token(parser, maybe_unit_token);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(unit_heading);
    assert(node != NULL);

    plang_token_t identifier = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(identifier, plang_token_type_identifier)) {
        plang_error_t error
            = plang_error_new(plang_error_type_expected_identifier,
                              plang_parser_get_source(parser),
                              plang_token_get_range(identifier));
        plang_parser_signal_error(parser, error);
        goto bail_out;
    }
    node->_identifier = identifier;

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_unit_heading_free(struct plang_node_unit_heading *ns)
{
    /* No sub-nodes. */
}

plang_node_t PLANG_NULLABLE
plang_node_interface_part_parse(plang_parser_t parser)
{
    struct plang_node_interface_part *node = NULL;

    plang_token_t maybe_interface = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_interface,
                             plang_token_type_INTERFACE))
    {
        /* Just bail out to backtrack. */
        if (maybe_interface) plang_parser_return_token(parser,
                                                maybe_interface);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(interface_part);
    assert(node != NULL);

    node->_uses_clause = plang_node_uses_clause_parse(parser);
    node->_constant_declaration_part = plang_node_constant_declaration_part_parse(parser);
    node->_type_declaration_part = plang_node_type_declaration_part_parse(parser);
    node->_variable_declaration_part = plang_node_variable_declaration_part_parse(parser);
    node->_procedure_and_function_declaration_part = plang_node_procedure_and_function_declaration_part_parse(parser, false);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_interface_part_free(struct plang_node_interface_part *ns)
{
    plang_node_free(ns->_uses_clause);
    plang_node_free(ns->_constant_declaration_part);
    plang_node_free(ns->_type_declaration_part);
    plang_node_free(ns->_variable_declaration_part);
    plang_node_free(ns->_procedure_and_function_declaration_part);
}


plang_node_t PLANG_NULLABLE
plang_node_implementation_part_parse(plang_parser_t parser)
{
    struct plang_node_implementation_part *node = NULL;

    plang_token_t maybe_implementation = plang_parser_next_significant_token(parser);
    if (!plang_token_matches(maybe_implementation,
                             plang_token_type_IMPLEMENTATION))
    {
        if (maybe_implementation)
            plang_parser_return_token(parser, maybe_implementation);
        goto bail_out;
    }

    node = PLANG_NODE_NEW(implementation_part);
    assert(node != NULL);

    node->_uses_clause = plang_node_uses_clause_parse(parser);
    node->_constant_declaration_part = plang_node_constant_declaration_part_parse(parser);
    node->_type_declaration_part = plang_node_type_declaration_part_parse(parser);
    node->_variable_declaration_part = plang_node_variable_declaration_part_parse(parser);
    node->_subroutine_part = plang_node_subroutine_part_parse(parser);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_implementation_part_free(struct plang_node_implementation_part *ns)
{
    plang_node_free(ns->_uses_clause);
    plang_node_free(ns->_constant_declaration_part);
    plang_node_free(ns->_type_declaration_part);
    plang_node_free(ns->_variable_declaration_part);
    plang_node_free(ns->_subroutine_part);
}


plang_node_t PLANG_NULLABLE
plang_node_subroutine_part_parse(plang_parser_t parser)
{
    struct plang_node_subroutine_part *node = NULL;

    node = PLANG_NODE_NEW(subroutine_part);
    assert(node != NULL);

    plang_array_t declarations = plang_array_new(4);
    assert(declarations != NULL);
    node->_declarations = declarations;

    bool first_time = true;
    bool done = false;
    do {
        plang_node_t declaration = NULL;

        declaration = plang_node_procedure_declaration_parse(parser,
                                                             true);

        if (declaration == NULL) {
            declaration = plang_node_function_declaration_parse(parser,
                                                                true);
        }

        if (declaration == NULL) {
            if (first_time) {
                /* Not what we're looking for, bail out to backtrack. */
                goto bail_out;
            } else {
                /* Just be done. */
                done = true;
            }
        } else {
            bool appended = plang_array_append(declarations,
                                               declaration);
            assert(appended != false);
        }
        if (first_time) first_time = false;
    } while (!done);

    return (plang_node_t) node;

bail_out:
    plang_node_free((plang_node_t) node);
    return NULL;
}

void
plang_node_subroutine_part_free(struct plang_node_subroutine_part *ns)
{
    plang_node_array_free(ns->_declarations);
}


PLANG_SOURCE_END
