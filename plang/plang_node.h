/*
    plang_node.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_node__h__
#define __plang_node__h__

#include "plang_defines.h"

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_parser *plang_parser_t;
typedef struct plang_scope *plang_scope_t;
typedef struct plang_token *plang_token_t;


/*!
 The types of syntax nodes in `plang`.

 - NOTE: The enumeration is named with the suffix `_e` so we can use the
         prettier name for the structure representing a type within the
         language `plang` implements.
 */
enum plang_node_type_e {
    plang_node_type_unknown = 0,

    /* Constants */

    plang_node_type_unsigned_integer,
    plang_node_type_unsigned_real,
    plang_node_type_scale_factor,
    plang_node_type_unsigned_number,
    plang_node_type_signed_number,
    plang_node_type_constant_declaration,
    plang_node_type_constant,
    plang_node_type_constant_identifier,
    plang_node_type_sign,

    /* Blocks */

    plang_node_type_block,
    plang_node_type_label_declaration_part,
    plang_node_type_label,
    plang_node_type_constant_declaration_part,
    plang_node_type_type_declaration_part,
    plang_node_type_variable_declaration_part,
    plang_node_type_procedure_and_function_declaration_part,
    plang_node_type_statement_part,

    /* Types */

    plang_node_type_type_declaration,
    plang_node_type_type_identifier,
    plang_node_type_string_type,
    plang_node_type_enumerated_type,
    plang_node_type_identifier_list,
    plang_node_type_subrange_type,
    plang_node_type_array_type,
    plang_node_type_index_type,
    plang_node_type_set_type,
    /* No file type. */
    plang_node_type_record_type,
    plang_node_type_field_list,
    plang_node_type_field_declaration,
    plang_node_type_fixed_part,
    plang_node_type_variant_part,
    plang_node_type_variant,
    plang_node_type_tag_field_type,
    plang_node_type_pointer_type,

    /* Variables */

    plang_node_type_variable_declaration,
    plang_node_type_variable_reference,
    plang_node_type_variable_identifier,
    plang_node_type_index,
    plang_node_type_field_designator,
    /* No file type. */
    plang_node_type_pointer_object_symbol,

    /* Expressions */

    plang_node_type_expression,
    plang_node_type_address_of,
    plang_node_type_unsigned_constant,
    plang_node_type_parenthesized_expression,
    plang_node_type_antifactor,
    plang_node_type_term,
    plang_node_type_simple_expression,
    plang_node_type_function_call,
    plang_node_type_actual_parameter_list,
    plang_node_type_actual_parameter,
    plang_node_type_set_constructor,
    plang_node_type_member_group,

    /* Statements */

    plang_node_type_statement,
    plang_node_type_statement_label,
    plang_node_type_assignment_statement,
    plang_node_type_procedure_statement,
    plang_node_type_goto_statement,
    plang_node_type_compound_statement,
    plang_node_type_if_statement,
    plang_node_type_case_statement,
    plang_node_type_case_clause,
    plang_node_type_otherwise_clause,
    plang_node_type_repeat_statement,
    plang_node_type_while_statement,
    plang_node_type_for_statement,
    plang_node_type_control_variable,
    plang_node_type_initial_value,
    plang_node_type_final_value,
    plang_node_type_record_variable_reference,
    plang_node_type_with_statement,

    /* Procedures & Functions */

    plang_node_type_procedure_identifier,
    plang_node_type_function_identifier,
    plang_node_type_procedure_reference,
    plang_node_type_function_reference,
    plang_node_type_procedure_declaration,
    plang_node_type_procedure_heading,
    plang_node_type_procedure_body,
    plang_node_type_function_declaration,
    plang_node_type_function_heading,
    plang_node_type_result_type,
    plang_node_type_function_body,
    plang_node_type_formal_parameter_list,
    plang_node_type_parameter_declaration,

    /* Programs */

    plang_node_type_program,
    plang_node_type_program_heading,
    plang_node_type_uses_clause,

    /* Units */

    plang_node_type_unit,
    plang_node_type_unit_heading,
    plang_node_type_interface_part,
    plang_node_type_implementation_part,
    plang_node_type_subroutine_part,
};
typedef enum plang_node_type_e plang_node_type_t;


/*!
 A parser syntax node.
 */
typedef struct plang_node *plang_node_t;


/*! Allocate a syntax node of a given type. */
plang_node_t PLANG_NULLABLE
plang_node_new(plang_node_type_t type);


/*! Dispose of a syntax node. */
void
plang_node_free(plang_node_t PLANG_NULLABLE node);


/*! Get the type of the given syntax node. */
plang_node_type_t
plang_node_get_type(plang_node_t node);


/*! Parse the topmost node of the abstract syntax tree. */
plang_node_t PLANG_NULLABLE
plang_node_topmost_parse(plang_parser_t parser);


/*!
 Get a type's declaration node.

 Gets the declaration node for a given type node.

 - NOTE: Returns `NULL` for built-in types.
 */
plang_node_t PLANG_NULLABLE
plang_node_type_get_type_declaration(plang_node_t type_node);


/*! Get the type part of a type declaration. */
plang_node_t PLANG_NULLABLE
plang_node_type_declaration_get_type(plang_node_t node);


/*! Get the identifier part of a type declaration. */
plang_token_t
plang_node_type_declaration_get_identifier(plang_node_t node);


/*! Get the identifier token for a type identifier node. */
plang_token_t
plang_node_type_identifier_get_identifier(plang_node_t PLANG_NULLABLE node);


/*!
 Get the type node for a _variable-declaration_ node.
 */
plang_node_t
plang_node_variable_declaration_get_type(plang_node_t node);


/*!
 Get the type node for a _variable-reference_ node.

 Whether the variable reference is via a variable identifier or via a
 function call, figure out the corresponding actual type node and return
 that.

 - NOTE: This takes a scope because determining the type of a variable
         must be done within a scope.
 */
plang_node_t
plang_node_variable_reference_get_type(plang_node_t node,
                                       plang_scope_t scope);


/*! Get the _result-type_ node for a _function-heading_ node. */
plang_node_t
plang_node_function_heading_get_result_type(plang_node_t node);


/*! Get the type for a _result-type_ node. */
plang_node_t
plang_node_result_type_get_type(plang_node_t node);


/*! Get the identifier for a program node. */
plang_token_t
plang_node_program_get_identifier(plang_node_t node);


/*! Get the scope for a program node. */
plang_scope_t
plang_node_program_get_scope(plang_node_t node);


/*! Get the identifier for a unit node. */
plang_token_t
plang_node_unit_get_identifier(plang_node_t node);


/*! Get the interface scope for a unit node. */
plang_scope_t
plang_node_unit_get_interface_scope(plang_node_t node);


/*! Get the implementation scope for a unit node. */
plang_scope_t
plang_node_unit_get_implementation_scope(plang_node_t node);


/*!
 Get the scope for a procedure node.

 - NOTE: This is polymorphic and will work for both procedure
         declarations and procedure headings.
 */
plang_scope_t
plang_node_procedure_get_scope(plang_node_t node);


/*!
 Get the scope for a function node.

 - NOTE: This is polymorphic and will work for both function
         declarations and procedure headings.
 */
plang_scope_t
plang_node_function_get_scope(plang_node_t node);


PLANG_HEADER_END

#endif /* __plang_node__h__ */
