/*
    plang_error.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_error__h__
#define __plang_error__h__

#include "plang_defines.h"

#include "plang_range.h"

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_source *plang_source_t;


/*!
 An error during tokenization or parsing.
 */
typedef struct plang_error *plang_error_t;


/*!
 Types of parse errors.
 */
enum plang_error_type {
    /*! Unknown type of error. */
    plang_error_type_unknown = 0,

    /* Tokenizer errors */

    /*! The input text ended unexpectedly. */
    plang_error_type_unexpected_end_of_text,

    /*! An input character was unexpected. */
    plang_error_type_unexpected_character,

    /* Token errors */

    plang_error_type_unexpected_token,

    plang_error_type_expected_SEMICOLON,
    plang_error_type_expected_COLON,
    plang_error_type_expected_PERIOD,
    plang_error_type_expected_LPAREN,
    plang_error_type_expected_RPAREN,
    plang_error_type_expected_LBRACKET,
    plang_error_type_expected_RBRACKET,
    plang_error_type_expected_EQUALS,
    plang_error_type_expected_ASSIGNMENT,
    plang_error_type_expected_RANGE,

    plang_error_type_expected_DO,
    plang_error_type_expected_END,
    plang_error_type_expected_LABEL,
    plang_error_type_expected_OF,
    plang_error_type_expected_THEN,

    plang_error_type_expected_UNIT,
    plang_error_type_expected_PROGRAM_or_UNIT,

    plang_error_type_expected_digit_sequence,

    /* Object Already Exists errors */

    plang_error_type_already_exists_constant,
    plang_error_type_already_exists_type,
    plang_error_type_already_exists_variable,
    plang_error_type_already_exists_procedure,
    plang_error_type_already_exists_function,

    /* Expected Type errors */

    plang_error_type_expected_type,
    plang_error_type_expected_result_type,
    plang_error_type_expected_array_set_or_record,
    plang_error_type_expected_index_type,
    plang_error_type_expected_ordinal_type,
    plang_error_type_expected_tag_field_type,

    /* Expected Node errors */

    plang_error_type_expected_statement,
    plang_error_type_expected_expression,
    plang_error_type_expected_identifier,
    plang_error_type_expected_type_identifier,
    plang_error_type_expected_variable_identifier,
    plang_error_type_expected_identifier_list,
    plang_error_type_expected_interface_part,
    plang_error_type_expected_implementation_part,
    plang_error_type_expected_statement_part,
    plang_error_type_expected_block,
    plang_error_type_expected_constant,
    plang_error_type_expected_declaration_or_block,
    plang_error_type_expected_term,
    plang_error_type_expected_factor,
    plang_error_type_expected_procedure_body,
    plang_error_type_expected_function_body,
    plang_error_type_expected_variable_reference,
    plang_error_type_expected_record_variable_reference,
    plang_error_type_expected_label,
    plang_error_type_expected_member_group,
    plang_error_type_expected_actual_parameter,
    plang_error_type_expected_unsigned_integer,
    plang_error_type_expected_control_variable,
    plang_error_type_expected_initial_value_expression,
    plang_error_type_expected_final_value_expression,
    plang_error_type_expected_field_declaration,
    plang_error_type_expected_parameter_declaration,
    plang_error_type_expected_simple_or_structured_statement,
    plang_error_type_expected_compound_statement,
    plang_error_type_expected_variant,
    plang_error_type_expected_simple_expression,
    plang_error_type_expected_fixed_part,
    plang_error_type_expected_variant_part,
};
typedef enum plang_error_type plang_error_type_t;


/*!
 Create an error.
 */
plang_error_t PLANG_NULLABLE
plang_error_new(plang_error_type_t type,
                plang_source_t source,
                plang_range_t range);


/*!
 Dispose of an error.
 */
void
plang_error_free(plang_error_t PLANG_NULLABLE error);


/*! Get the type of an error. */
plang_error_type_t
plang_error_get_type(plang_error_t error);


/*! Get the source containing an error. */
plang_source_t
plang_error_get_source(plang_error_t error);


/*! Get the range of an error. */
plang_range_t
plang_error_get_range(plang_error_t error);


/*!
 Get the message for an error.

 - WARNING: Returns memory allocated with ``calloc(3)`` that the caller
            must release with ``free(3)``.
 */
const char * PLANG_NULLABLE
plang_error_copy_text(plang_error_t error);


PLANG_HEADER_END

#endif /* __plang_error__h__ */
