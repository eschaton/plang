/*
    plang_error.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_error_internal.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plang_source.h"

PLANG_SOURCE_BEGIN


plang_error_t PLANG_NULLABLE
plang_error_new(plang_error_type_t type,
                plang_source_t source,
                plang_range_t range)
{
    plang_error_t error = calloc(sizeof(struct plang_error), 1);
    if (error) {
        error->_type = type;
        error->_source = source;
        error->_range = range;
    }

    return error;
}


void
plang_error_free(plang_error_t PLANG_NULLABLE error)
{
    if (error) {
        free(error);
    }
}


plang_error_type_t
plang_error_get_type(plang_error_t error)
{
    return error->_type;
}


plang_source_t
plang_error_get_source(plang_error_t error)
{
    return error->_source;
}


plang_range_t
plang_error_get_range(plang_error_t error)
{
    return error->_range;
}


const char *
plang_error_copy_text(plang_error_t error)
{
    /*
     An error message has the following format:

         file:start:length: error: message

     The file can vary in size, the start and length can each be up to
     log10(SIZE_MAX)=20 digits in size, and the message itself can vary
     in size.
     */

    const char *file = plang_source_get_vpath(error->_source);

    const char * const message
        = plang_error_message_for_type(error->_type);

    const size_t source_len = strlen(file);
    const size_t start_len = 20;
    const size_t length_len = 20;
    const size_t message_len = strlen(message);
    const size_t overhead = (4 /* colons */ + 2 /* spaces */ +
                             5 /* "error" */ + 1 /* terminator */);
    const size_t buf_len = (source_len + start_len + length_len +
                            message_len + overhead);

    char *buf = calloc(sizeof(char), buf_len);
    if (buf == NULL) return NULL;

    snprintf(buf, buf_len, "%s:%zu:%zu: error: %s", file,
             error->_range.start, error->_range.length, message);

    return buf;
}


const char * const
plang_error_message_for_type(plang_error_type_t type)
{
    /*
     Do not add a default: clause to this, so adding a new error type
     causes a compiler warning here.
     */
    switch (type) {

        case plang_error_type_unknown:
            return "Unknown error";

            /* Tokenizer errors */

        case plang_error_type_unexpected_end_of_text:
            return "Unexpected end of text";

        case plang_error_type_unexpected_character:
            return "Unexpected input character";


            /* Token errors */

        case plang_error_type_unexpected_token:
            return "Unexpected token";

        case plang_error_type_expected_SEMICOLON:
            return "Expected semicolon ';'";

        case plang_error_type_expected_COLON:
            return "Expected colon ':'";

        case plang_error_type_expected_PERIOD:
            return "Expected period '.'";

        case plang_error_type_expected_LPAREN:
            return "Expected left parenthesis '('";

        case plang_error_type_expected_RPAREN:
            return "Expected right parenthesis ')'";

        case plang_error_type_expected_LBRACKET:
            return "Expected left bracket '['";

        case plang_error_type_expected_RBRACKET:
            return "Expected right bracket ']'";

        case plang_error_type_expected_EQUALS:
            return "Expected equals '='";

        case plang_error_type_expected_ASSIGNMENT:
            return "Expected assignment ':='";

        case plang_error_type_expected_RANGE:
            return "Expected range '..'";


        case plang_error_type_expected_DO:
            return "Expected DO token";

        case plang_error_type_expected_END:
            return "Expected END token";

        case plang_error_type_expected_LABEL:
            return "Expected LABEL token";

        case plang_error_type_expected_OF:
            return "Expected OF token";

        case plang_error_type_expected_THEN:
            return "Expected THEN token";

        case plang_error_type_expected_UNIT:
            return "Expected UNIT token";

        case plang_error_type_expected_PROGRAM_or_UNIT:
            return "Expected PROGRAM or UNIT token";

        case plang_error_type_expected_digit_sequence:
            return "Expected digit sequence";


            /* Object Already Exists errors */

        case plang_error_type_already_exists_constant:
            return "Constant already exists";

        case plang_error_type_already_exists_type:
            return "Type already exists";

        case plang_error_type_already_exists_variable:
            return "Variable already exists";

        case plang_error_type_already_exists_procedure:
            return "Procedure already exists";

        case plang_error_type_already_exists_function:
            return "Function already exists";


            /* Expected Type errors */

        case plang_error_type_expected_type:
            return "Expected type";

        case plang_error_type_expected_result_type:
            return "Expected result type";

        case plang_error_type_expected_array_set_or_record:
            return "Expected array, set, or record type";

        case plang_error_type_expected_index_type:
            return "Expected index-type";

        case plang_error_type_expected_ordinal_type:
            return "Expected ordinal-type";

        case plang_error_type_expected_tag_field_type:
            return "Expected tag-field-type";


            /* Expected Node errors */
        case plang_error_type_expected_expression:
            return "Expected expression";

        case plang_error_type_expected_statement:
            return "Expected statement";

        case plang_error_type_expected_identifier:
            return "Expected identifier";

        case plang_error_type_expected_type_identifier:
            return "Expected type identifier";

        case plang_error_type_expected_variable_identifier:
            return "Expected variable identifier";

        case plang_error_type_expected_identifier_list:
            return "Expected identifier list";

        case plang_error_type_expected_interface_part:
            return "Expected interface part";

        case plang_error_type_expected_implementation_part:
            return "Expected implementation part";

        case plang_error_type_expected_statement_part:
            return "Expected statement part";

        case plang_error_type_expected_block:
            return "Expected block";

        case plang_error_type_expected_constant:
            return "Expected constant";

        case plang_error_type_expected_declaration_or_block:
            return "Expected declaration or block";

        case plang_error_type_expected_term:
            return "Expected term";

        case plang_error_type_expected_factor:
            return "Expected factor";

        case plang_error_type_expected_procedure_body:
            return "Expected procedure body";

        case plang_error_type_expected_function_body:
            return "Expected function body";

        case plang_error_type_expected_variable_reference:
            return "Expected variable reference";

        case plang_error_type_expected_record_variable_reference:
            return "Expected record variable reference";

        case plang_error_type_expected_label:
            return "Expected label";

        case plang_error_type_expected_member_group:
            return "Expected member group";

        case plang_error_type_expected_actual_parameter:
            return "Expected actual parameter";

        case plang_error_type_expected_unsigned_integer:
            return "Expected unsigned integer";

        case plang_error_type_expected_control_variable:
            return "Expected control variable";

        case plang_error_type_expected_initial_value_expression:
            return "Expected initial-value expression";

        case plang_error_type_expected_final_value_expression:
            return "Expected final-value expression";

        case plang_error_type_expected_field_declaration:
            return "Expected field declaration";

        case plang_error_type_expected_parameter_declaration:
            return "Expected parameter declaration";

        case plang_error_type_expected_simple_or_structured_statement:
            return "Expected simple or compound statement";

        case plang_error_type_expected_compound_statement:
            return "Expected compound statement";

        case plang_error_type_expected_variant:
            return "Expected variant";

        case plang_error_type_expected_simple_expression:
            return "Expected simple-expression";

#if PLANG_CLASCAL
        case plang_error_type_expected_class_identifier:
            return "Expected class identifier";

        case plang_error_type_expected_class_instance:
            return "Expected class instance";

        case plang_error_type_expected_procedure_call:
            return "Expected procedure call";

        case plang_error_type_expected_function_call:
            return "Expected function call";

        case plang_error_type_expected_creation_block:
            return "Expected creation block";
#endif
    }
}


PLANG_SOURCE_END
