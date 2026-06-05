/*
    plang_node_internal.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_node_internal__h__
#define __plang_node_internal__h__

#include "plang_node.h"

#include <stdbool.h>

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_array *plang_array_t;
typedef struct plang_token *plang_token_t;


/*!
 An abstract node in the abstract syntax tree.

 This is both the currency type for passing around nodes and the header
 of an individual node.
 */
struct plang_node {
    plang_node_type_t _type;
};


/*! Convenience for declaring ``plang_node`` `_parse` functions. */
#define PLANG_NODE_PARSE_PROTOTYPE(t) \
    plang_node_t PLANG_NULLABLE \
    plang_node_ ## t ## _parse(plang_parser_t parser)


/*! Convenience for declaring ``plang_node`` `_unparse` functions. */
#define PLANG_NODE_UNPARSE_PROTOTYPE(t) \
    void \
    plang_node_ ## t ## _unparse(plang_parser_t parser, \
                                 plang_node_t node)


/*! Convenience for declaring ``plang_node`` `_free` functions. */
#define PLANG_NODE_FREE_PROTOTYPE(t) \
    void \
    plang_node_ ## t ## _free(struct plang_node_ ## t *ns)


/* MARK: - Constants */

struct plang_node_unsigned_integer {
    struct plang_node _header;

    plang_token_t PLANG_NULLABLE _digit_sequence;
    plang_token_t PLANG_NULLABLE _hex_digit_sequence;
};

PLANG_NODE_PARSE_PROTOTYPE(unsigned_integer);
PLANG_NODE_FREE_PROTOTYPE(unsigned_integer);

struct plang_node_unsigned_real {
    struct plang_node _header;

    plang_token_t _whole_part;
    plang_token_t PLANG_NULLABLE _fractional_part;
    plang_node_t PLANG_NULLABLE _scale_factor;
};

PLANG_NODE_PARSE_PROTOTYPE(unsigned_real);
PLANG_NODE_FREE_PROTOTYPE(unsigned_real);

struct plang_node_scale_factor {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _sign;
    plang_token_t _digit_sequence;
};

PLANG_NODE_PARSE_PROTOTYPE(scale_factor);
PLANG_NODE_FREE_PROTOTYPE(scale_factor);

struct plang_node_unsigned_number {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _unsigned_integer;
    plang_node_t PLANG_NULLABLE _unsigned_real;
};

PLANG_NODE_PARSE_PROTOTYPE(unsigned_number);
PLANG_NODE_FREE_PROTOTYPE(unsigned_number);

struct plang_node_signed_number {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _sign;
    plang_node_t _unsigned_number;
};

PLANG_NODE_PARSE_PROTOTYPE(signed_number);
PLANG_NODE_FREE_PROTOTYPE(signed_number);

struct plang_node_constant_declaration {
    struct plang_node _header;

    plang_token_t _identifier;
    plang_node_t _constant;
};

PLANG_NODE_PARSE_PROTOTYPE(constant_declaration);
PLANG_NODE_FREE_PROTOTYPE(constant_declaration);

struct plang_node_constant {
    struct plang_node _header;

    /* Only one of the following sections is non-NULL. */

    plang_node_t PLANG_NULLABLE _sign;
    plang_node_t PLANG_NULLABLE _constant_identifier;

    plang_node_t PLANG_NULLABLE _signed_number;

    plang_token_t PLANG_NULLABLE _quoted_string;

    /* No quoted-char. */
};

PLANG_NODE_PARSE_PROTOTYPE(constant);
PLANG_NODE_FREE_PROTOTYPE(constant);

struct plang_node_constant_identifier {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(constant_identifier);
PLANG_NODE_FREE_PROTOTYPE(constant_identifier);

struct plang_node_sign {
    struct plang_node _header;

    plang_token_t _token;
};

plang_node_t PLANG_NULLABLE
plang_node_sign_parse(plang_parser_t parser,
                      bool allow_whitespace);
PLANG_NODE_UNPARSE_PROTOTYPE(sign);
PLANG_NODE_FREE_PROTOTYPE(sign);


/* MARK: - Blocks */

struct plang_node_block {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _label_declaration_part;
    plang_node_t PLANG_NULLABLE _constant_declaration_part;
    plang_node_t PLANG_NULLABLE _type_declaration_part;
    plang_node_t PLANG_NULLABLE _variable_declaration_part;
    plang_node_t PLANG_NULLABLE _procedure_and_function_declaration_part;
    plang_node_t PLANG_NULLABLE _statement_part;
};

PLANG_NODE_PARSE_PROTOTYPE(block);
PLANG_NODE_FREE_PROTOTYPE(block);

struct plang_node_label_declaration_part {
    struct plang_node _header;

    plang_array_t _labels;
};

PLANG_NODE_PARSE_PROTOTYPE(label_declaration_part);
PLANG_NODE_FREE_PROTOTYPE(label_declaration_part);

struct plang_node_label {
    struct plang_node _header;

    plang_token_t _digit_sequence;
};

PLANG_NODE_PARSE_PROTOTYPE(label);
PLANG_NODE_FREE_PROTOTYPE(label);

struct plang_node_constant_declaration_part {
    struct plang_node _header;

    plang_array_t _constant_declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(constant_declaration_part);
PLANG_NODE_FREE_PROTOTYPE(constant_declaration_part);

struct plang_node_type_declaration_part {
    struct plang_node _header;

    plang_array_t _type_declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(type_declaration_part);
PLANG_NODE_FREE_PROTOTYPE(type_declaration_part);

struct plang_node_variable_declaration_part {
    struct plang_node _header;

    plang_array_t _variable_declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(variable_declaration_part);
PLANG_NODE_FREE_PROTOTYPE(variable_declaration_part);

struct plang_node_procedure_and_function_declaration_part {
    struct plang_node _header;

    plang_array_t _procedure_and_function_declarations;
};

plang_node_t PLANG_NULLABLE
plang_node_procedure_and_function_declaration_part_parse(plang_parser_t parser,
                                                         bool allow_body);
PLANG_NODE_FREE_PROTOTYPE(procedure_and_function_declaration_part);

struct plang_node_statement_part {
    struct plang_node _header;

    plang_node_t _compound_statement;
};

PLANG_NODE_PARSE_PROTOTYPE(statement_part);
PLANG_NODE_FREE_PROTOTYPE(statement_part);


/* MARK: - Types */

struct plang_node_type_declaration {
    struct plang_node _header;

    plang_token_t _identifier;
    plang_node_t _type;
};

PLANG_NODE_PARSE_PROTOTYPE(type_declaration);
PLANG_NODE_FREE_PROTOTYPE(type_declaration);

/*!
 Set a type's declaration node.

 Sets the declaration node for a given type node.
 */
void
plang_node_type_set_type_declaration(plang_node_t type_node,
                                     plang_node_t declaration_node);

PLANG_NODE_PARSE_PROTOTYPE(type);
PLANG_NODE_PARSE_PROTOTYPE(simple_type);
/* No free, these are meta-nodes. */

struct plang_node_type_identifier {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(type_identifier);
PLANG_NODE_FREE_PROTOTYPE(type_identifier);

PLANG_NODE_PARSE_PROTOTYPE(simple_type_identifier);
PLANG_NODE_PARSE_PROTOTYPE(structured_type_identifier);
PLANG_NODE_PARSE_PROTOTYPE(pointer_type_identifier);
PLANG_NODE_PARSE_PROTOTYPE(ordinal_type_identifier);
PLANG_NODE_PARSE_PROTOTYPE(real_type_identifier);
PLANG_NODE_PARSE_PROTOTYPE(string_type_identifier);
/* No free, these are meta-nodes. */

struct plang_node_subrange_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_node_t _start;
    plang_node_t _end;
};

PLANG_NODE_PARSE_PROTOTYPE(subrange_type);
PLANG_NODE_FREE_PROTOTYPE(subrange_type);

PLANG_NODE_PARSE_PROTOTYPE(ordinal_type);
/* No free, this is a meta-node. */

bool
plang_node_is_ordinal_type(plang_node_t node);

struct plang_node_string_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_node_t PLANG_NULLABLE _size_attribute;
};

PLANG_NODE_PARSE_PROTOTYPE(string_type);
PLANG_NODE_FREE_PROTOTYPE(string_type);

struct plang_node_enumerated_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_node_t _identifier_list;
};

PLANG_NODE_PARSE_PROTOTYPE(enumerated_type);
PLANG_NODE_FREE_PROTOTYPE(enumerated_type);

struct plang_node_identifier_list {
    struct plang_node _header;

    plang_array_t _identifiers;
};

PLANG_NODE_PARSE_PROTOTYPE(identifier_list);
PLANG_NODE_FREE_PROTOTYPE(identifier_list);

struct plang_node_structured_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_token_t _structured_type_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(structured_type);
PLANG_NODE_FREE_PROTOTYPE(structured_type);

struct plang_node_array_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    bool _is_packed;
    plang_array_t _index_types;
    plang_node_t _type;
};

plang_node_t PLANG_NULLABLE
plang_node_array_type_parse(plang_parser_t parser,
                            bool is_packed);
PLANG_NODE_FREE_PROTOTYPE(array_type);

struct plang_node_index_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_node_t _ordinal_type;
};

PLANG_NODE_PARSE_PROTOTYPE(index_type);
PLANG_NODE_FREE_PROTOTYPE(index_type);

struct plang_node_set_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    bool _is_packed;
    plang_node_t _ordinal_type;
};

plang_node_t PLANG_NULLABLE
plang_node_set_type_parse(plang_parser_t parser,
                          bool is_packed);
PLANG_NODE_FREE_PROTOTYPE(set_type);

/* No file type. */

struct plang_node_record_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    bool _is_packed;
    plang_node_t PLANG_NULLABLE _field_list;
};

plang_node_t PLANG_NULLABLE
plang_node_record_type_parse(plang_parser_t parser,
                             bool is_packed);
PLANG_NODE_FREE_PROTOTYPE(record_type);

struct plang_node_field_list {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _fixed_part;
    plang_node_t PLANG_NULLABLE _variant_part;
};

PLANG_NODE_PARSE_PROTOTYPE(field_list);
PLANG_NODE_FREE_PROTOTYPE(field_list);

struct plang_node_field_declaration {
    struct plang_node _header;

    plang_node_t _identifier_list;
    plang_node_t _type;
};

PLANG_NODE_PARSE_PROTOTYPE(field_declaration);
PLANG_NODE_FREE_PROTOTYPE(field_declaration);

struct plang_node_fixed_part {
    struct plang_node _header;

    plang_array_t _field_declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(fixed_part);
PLANG_NODE_FREE_PROTOTYPE(fixed_part);

struct plang_node_variant_part {
    struct plang_node _header;

    plang_token_t PLANG_NULLABLE _identifier;
    plang_node_t _tag_field_type;
    plang_array_t _variants;
};

PLANG_NODE_PARSE_PROTOTYPE(variant_part);
PLANG_NODE_FREE_PROTOTYPE(variant_part);

struct plang_node_tag_field_type {
    struct plang_node _header;

    plang_node_t _ordinal_type_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(tag_field_type);
PLANG_NODE_FREE_PROTOTYPE(tag_field_type);

struct plang_node_variant {
    struct plang_node _header;

    plang_array_t _constants;
    plang_node_t PLANG_NULLABLE _field_list;
};

PLANG_NODE_PARSE_PROTOTYPE(variant);
PLANG_NODE_FREE_PROTOTYPE(variant);

struct plang_node_pointer_type {
    struct plang_node _header;
    plang_node_t _declaration;		/*!< backwards link */

    plang_node_t PLANG_NULLABLE _type_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(pointer_type);
PLANG_NODE_FREE_PROTOTYPE(pointer_type);


/* MARK: - Variables */

struct plang_node_variable_declaration {
    struct plang_node _header;

    plang_node_t _identifier_list;
    plang_node_t _type;
};

PLANG_NODE_PARSE_PROTOTYPE(variable_declaration);
PLANG_NODE_FREE_PROTOTYPE(variable_declaration);

struct plang_node_variable_reference {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _variable_identifier;
    plang_node_t PLANG_NULLABLE _function_call;
    plang_array_t PLANG_NULLABLE _qualifiers;
};

PLANG_NODE_PARSE_PROTOTYPE(variable_reference);
PLANG_NODE_FREE_PROTOTYPE(variable_reference);

struct plang_node_variable_identifier {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(variable_identifier);
PLANG_NODE_FREE_PROTOTYPE(variable_identifier);

PLANG_NODE_PARSE_PROTOTYPE(qualifier);
/* No free, qualifier is a virtual node. */

struct plang_node_index {
    struct plang_node _header;

    plang_array_t _expressions;
};

PLANG_NODE_PARSE_PROTOTYPE(index);
PLANG_NODE_FREE_PROTOTYPE(index);

struct plang_node_field_designator {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(field_designator);
PLANG_NODE_FREE_PROTOTYPE(field_designator);

/* No file type. */

struct plang_node_pointer_object_symbol {
    struct plang_node _header;

    plang_token_t _circumflex;
};

PLANG_NODE_PARSE_PROTOTYPE(pointer_object_symbol);
PLANG_NODE_FREE_PROTOTYPE(pointer_object_symbol);


/* MARK: - Expressions */

struct plang_node_expression {
    struct plang_node _header;

    plang_node_t _left_expression;
    plang_token_t PLANG_NULLABLE _operator;
    plang_node_t PLANG_NULLABLE _right_expression;
};

PLANG_NODE_PARSE_PROTOTYPE(expression);
PLANG_NODE_FREE_PROTOTYPE(expression);

PLANG_NODE_PARSE_PROTOTYPE(factor);
/* No `_free`, factor is a meta-node. */

struct plang_node_address_of {
    struct plang_node _header;

    plang_node_t _variable_reference;
};

PLANG_NODE_PARSE_PROTOTYPE(address_of);
PLANG_NODE_FREE_PROTOTYPE(address_of);

struct plang_node_unsigned_constant {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _unsigned_number;
    plang_token_t PLANG_NULLABLE _quoted_string_constant;
    plang_token_t PLANG_NULLABLE _nil;

    plang_node_t PLANG_NULLABLE _constant_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(unsigned_constant);
PLANG_NODE_FREE_PROTOTYPE(unsigned_constant);

struct plang_node_parenthesized_expression {
    struct plang_node _header;

    plang_node_t _expression;
};

PLANG_NODE_PARSE_PROTOTYPE(parenthesized_expression);
PLANG_NODE_FREE_PROTOTYPE(parenthesized_expression);

struct plang_node_antifactor {
    struct plang_node _header;

    plang_node_t _factor;
};

PLANG_NODE_PARSE_PROTOTYPE(antifactor);
PLANG_NODE_FREE_PROTOTYPE(antifactor);

struct plang_node_term {
    struct plang_node _header;

    /*! Contents are ``plang_node_t`` */
    plang_array_t _factors;

    /*! Contents are ``plang_token_t``, one fewer than ``_factors``. */
    plang_array_t _operators;
};

PLANG_NODE_PARSE_PROTOTYPE(term);
PLANG_NODE_FREE_PROTOTYPE(term);

struct plang_node_simple_expression {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _sign;

    /*!
     The terms in a simple-expression.

     Contents are ``plang_node_t``.
     */
    plang_array_t _terms;

    /*!
     The operators between terms in a simple-expression.

     Contents are ``plang_token_t``, one fewer than ``_terms``.
     */
    plang_array_t _operators;
};

PLANG_NODE_PARSE_PROTOTYPE(simple_expression);
PLANG_NODE_FREE_PROTOTYPE(simple_expression);

struct plang_node_function_call {
    struct plang_node _header;

    plang_node_t _function_reference;
    plang_node_t PLANG_NULLABLE _actual_parameter_list;
};

PLANG_NODE_PARSE_PROTOTYPE(function_call);
PLANG_NODE_FREE_PROTOTYPE(function_call);

struct plang_node_actual_parameter_list {
    struct plang_node _header;

    /*! Collection of ``plang_node_t`` actual-parameters. */
    plang_array_t _actual_parameters;
};

PLANG_NODE_PARSE_PROTOTYPE(actual_parameter_list);
PLANG_NODE_FREE_PROTOTYPE(actual_parameter_list);

struct plang_node_actual_parameter {
    struct plang_node _header;

    plang_node_t _subnode;
};

PLANG_NODE_PARSE_PROTOTYPE(actual_parameter);
PLANG_NODE_FREE_PROTOTYPE(actual_parameter);

struct plang_node_set_constructor {
    struct plang_node _header;

    plang_array_t _member_groups;
};

PLANG_NODE_PARSE_PROTOTYPE(set_constructor);
PLANG_NODE_FREE_PROTOTYPE(set_constructor);

struct plang_node_member_group {
    struct plang_node _header;
    
    plang_node_t _start_expression;
    plang_node_t PLANG_NULLABLE _end_expression;
};

PLANG_NODE_PARSE_PROTOTYPE(member_group);
PLANG_NODE_FREE_PROTOTYPE(member_group);


/* MARK: - Statements */

struct plang_node_statement {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _statement_label;
    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(statement);
PLANG_NODE_FREE_PROTOTYPE(statement);

struct plang_node_statement_label {
    struct plang_node _header;

    plang_node_t _label;
};

PLANG_NODE_PARSE_PROTOTYPE(statement_label);
PLANG_NODE_FREE_PROTOTYPE(statement_label);

PLANG_NODE_PARSE_PROTOTYPE(simple_statement);

struct plang_node_assignment_statement {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _variable_reference;
    plang_node_t PLANG_NULLABLE _function_identifier;
    plang_node_t _expression;
};

PLANG_NODE_PARSE_PROTOTYPE(assignment_statement);
PLANG_NODE_FREE_PROTOTYPE(assignment_statement);

struct plang_node_procedure_statement {
    struct plang_node _header;

    plang_node_t _procedure_reference;
    plang_node_t PLANG_NULLABLE _actual_parameter_list;
};

PLANG_NODE_PARSE_PROTOTYPE(procedure_statement);
PLANG_NODE_FREE_PROTOTYPE(procedure_statement);

struct plang_node_goto_statement {
    struct plang_node _header;

    plang_node_t _label;
};

PLANG_NODE_PARSE_PROTOTYPE(goto_statement);
PLANG_NODE_FREE_PROTOTYPE(goto_statement);

PLANG_NODE_PARSE_PROTOTYPE(structured_statement);

struct plang_node_compound_statement {
    struct plang_node _header;

    plang_array_t _statements;
};

PLANG_NODE_PARSE_PROTOTYPE(compound_statement);
PLANG_NODE_FREE_PROTOTYPE(compound_statement);

PLANG_NODE_PARSE_PROTOTYPE(conditional_statement);

struct plang_node_if_statement {
    struct plang_node _header;

    plang_node_t _expression;
    plang_node_t _true_statement;
    plang_node_t PLANG_NULLABLE _false_statement;
};

PLANG_NODE_PARSE_PROTOTYPE(if_statement);
PLANG_NODE_FREE_PROTOTYPE(if_statement);

struct plang_node_case_statement {
    struct plang_node _header;

    plang_node_t _expression;
    plang_array_t _cases;
    plang_node_t PLANG_NULLABLE _otherwise_clause;
};

PLANG_NODE_PARSE_PROTOTYPE(case_statement);
PLANG_NODE_FREE_PROTOTYPE(case_statement);

struct plang_node_case_clause {
    struct plang_node _header;

    plang_array_t _constants;
    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(case_clause);
PLANG_NODE_FREE_PROTOTYPE(case_clause);

struct plang_node_otherwise_clause {
    struct plang_node _header;

    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(otherwise_clause);
PLANG_NODE_FREE_PROTOTYPE(otherwise_clause);

PLANG_NODE_PARSE_PROTOTYPE(repetitive_statement);

struct plang_node_repeat_statement {
    struct plang_node _header;

    plang_array_t _statements;
    plang_node_t _expression;
};

PLANG_NODE_PARSE_PROTOTYPE(repeat_statement);
PLANG_NODE_FREE_PROTOTYPE(repeat_statement);

struct plang_node_while_statement {
    struct plang_node _header;

    plang_node_t _expression;
    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(while_statement);
PLANG_NODE_FREE_PROTOTYPE(while_statement);

struct plang_node_for_statement {
    struct plang_node _header;

    bool _counting_down;
    plang_node_t _control_variable;
    plang_node_t _initial_value;
    plang_node_t _final_value;
    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(for_statement);
PLANG_NODE_FREE_PROTOTYPE(for_statement);

struct plang_node_control_variable {
    struct plang_node _header;

    plang_node_t _variable_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(control_variable);
PLANG_NODE_FREE_PROTOTYPE(control_variable);

struct plang_node_initial_value {
    struct plang_node _header;

    plang_node_t _expression;
};

PLANG_NODE_PARSE_PROTOTYPE(initial_value);
PLANG_NODE_FREE_PROTOTYPE(initial_value);

struct plang_node_final_value {
    struct plang_node _header;

    plang_node_t _expression;
};

PLANG_NODE_PARSE_PROTOTYPE(final_value);
PLANG_NODE_FREE_PROTOTYPE(final_value);

struct plang_node_record_variable_reference {
    struct plang_node _header;

    plang_node_t _variable_reference;
};

PLANG_NODE_PARSE_PROTOTYPE(record_variable_reference);
PLANG_NODE_FREE_PROTOTYPE(record_variable_reference);

struct plang_node_with_statement {
    struct plang_node _header;

    plang_array_t _record_variable_references;
    plang_node_t _statement;
};

PLANG_NODE_PARSE_PROTOTYPE(with_statement);
PLANG_NODE_FREE_PROTOTYPE(with_statement);


/* MARK: - Procedures & Functions */

struct plang_node_procedure_identifier {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(procedure_identifier);
PLANG_NODE_FREE_PROTOTYPE(procedure_identifier);

struct plang_node_function_identifier {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(function_identifier);
PLANG_NODE_FREE_PROTOTYPE(function_identifier);

struct plang_node_procedure_reference {
    struct plang_node _header;

#if PLANG_CLASCAL
    plang_node_t PLANG_NULLABLE _variable_reference;
    plang_node_t PLANG_NULLABLE _class_type_identifier;
#endif
    plang_node_t _procedure_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(procedure_reference);
PLANG_NODE_FREE_PROTOTYPE(procedure_reference);

struct plang_node_function_reference {
    struct plang_node _header;

#if PLANG_CLASCAL
    plang_node_t PLANG_NULLABLE _variable_reference;
    plang_node_t PLANG_NULLABLE _class_type_identifier;
#endif
    plang_node_t _function_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(function_reference);
PLANG_NODE_FREE_PROTOTYPE(function_reference);

struct plang_node_procedure_declaration {
    struct plang_node _header;

    plang_node_t _procedure_heading;
    plang_node_t _procedure_body;
};

plang_node_t PLANG_NULLABLE
plang_node_procedure_declaration_parse(plang_parser_t parser,
                                       bool allow_block);
PLANG_NODE_FREE_PROTOTYPE(procedure_declaration);

struct plang_node_procedure_heading {
    struct plang_node _header;

    plang_scope_t _scope;

#if PLANG_CLASCAL
    plang_node_t PLANG_NULLABLE _class_type_identifier;
#endif
    plang_token_t _identifier;
    plang_node_t PLANG_NULLABLE _formal_parameter_list;
};

PLANG_NODE_PARSE_PROTOTYPE(procedure_heading);
PLANG_NODE_FREE_PROTOTYPE(procedure_heading);

struct plang_node_procedure_body {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _block;
    bool _is_forward;
    bool _is_external;
};

plang_node_t PLANG_NULLABLE
plang_node_procedure_body_parse(plang_parser_t parser,
                                bool allow_block);
PLANG_NODE_FREE_PROTOTYPE(procedure_body);

struct plang_node_function_declaration {
    struct plang_node _header;

    plang_node_t _function_heading;
    plang_node_t _function_body;
};

plang_node_t PLANG_NULLABLE
plang_node_function_declaration_parse(plang_parser_t parser,
                                      bool allow_block);
PLANG_NODE_FREE_PROTOTYPE(function_declaration);

struct plang_node_function_heading {
    struct plang_node _header;

    plang_scope_t _scope;

#if PLANG_CLASCAL
    plang_node_t PLANG_NULLABLE _class_type_identifier;
#endif
    plang_token_t _identifier;
    plang_node_t PLANG_NULLABLE _formal_parameter_list;
    plang_node_t _result_type;
};

PLANG_NODE_PARSE_PROTOTYPE(function_heading);
PLANG_NODE_FREE_PROTOTYPE(function_heading);

struct plang_node_result_type {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _ordinal_type_identifier;
    plang_node_t PLANG_NULLABLE _real_type_identifier;
    plang_node_t PLANG_NULLABLE _pointer_type_identifier;
#if PLANG_CLASCAL
    plang_node_t PLANG_NULLABLE _class_type_identifier;
#endif
};

PLANG_NODE_PARSE_PROTOTYPE(result_type);
PLANG_NODE_FREE_PROTOTYPE(result_type);

struct plang_node_function_body {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _block;
    bool _is_forward;
    bool _is_external;
};

plang_node_t PLANG_NULLABLE
plang_node_function_body_parse(plang_parser_t parser,
                               bool allow_block);
PLANG_NODE_FREE_PROTOTYPE(function_body);

struct plang_node_formal_parameter_list {
    struct plang_node _header;

    /*!
     Function & procedure parameter list

     Each item is a ``plang_node_t`` that's one of:
     - parameter-declaration
     - procedure-heading
     - function-heading
     */
    plang_array_t _parameter_declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(formal_parameter_list);
PLANG_NODE_FREE_PROTOTYPE(formal_parameter_list);

struct plang_node_parameter_declaration {
    struct plang_node _header;

    bool _is_var;
    plang_node_t _identifier_list;
    plang_node_t _type_identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(parameter_declaration);
PLANG_NODE_FREE_PROTOTYPE(parameter_declaration);


/* MARK: - Programs */

struct plang_node_program {
    struct plang_node _header;

    plang_scope_t _scope;

    plang_node_t _program_heading;
    plang_node_t PLANG_NULLABLE _uses_clause;
    plang_node_t _block;
};

PLANG_NODE_PARSE_PROTOTYPE(program);
PLANG_NODE_FREE_PROTOTYPE(program);

struct plang_node_program_heading {
    struct plang_node _header;

    plang_token_t _identifier;
    /* No program-parameters in plang. */
};

PLANG_NODE_PARSE_PROTOTYPE(program_heading);
PLANG_NODE_FREE_PROTOTYPE(program_heading);

struct plang_node_uses_clause {
    struct plang_node _header;

    plang_node_t _identifier_list;
};

PLANG_NODE_PARSE_PROTOTYPE(uses_clause);
PLANG_NODE_FREE_PROTOTYPE(uses_clause);


/* MARK: - Units */

struct plang_node_unit {
    struct plang_node _header;

    plang_node_t _unit_heading;
    plang_node_t _interface_part;
    plang_node_t _implementation_part;

    plang_scope_t _interface_scope;
    plang_scope_t _implementation_scope;
};

PLANG_NODE_PARSE_PROTOTYPE(unit);
PLANG_NODE_FREE_PROTOTYPE(unit);

struct plang_node_unit_heading {
    struct plang_node _header;

    plang_token_t _identifier;
};

PLANG_NODE_PARSE_PROTOTYPE(unit_heading);
PLANG_NODE_FREE_PROTOTYPE(unit_heading);

struct plang_node_interface_part {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _uses_clause;
    plang_node_t PLANG_NULLABLE _constant_declaration_part;
    plang_node_t PLANG_NULLABLE _type_declaration_part;
    plang_node_t PLANG_NULLABLE _variable_declaration_part;
    plang_node_t PLANG_NULLABLE _procedure_and_function_declaration_part;
};

PLANG_NODE_PARSE_PROTOTYPE(interface_part);
PLANG_NODE_FREE_PROTOTYPE(interface_part);

struct plang_node_implementation_part {
    struct plang_node _header;

    plang_node_t PLANG_NULLABLE _uses_clause;
    plang_node_t PLANG_NULLABLE _constant_declaration_part;
    plang_node_t PLANG_NULLABLE _type_declaration_part;
    plang_node_t PLANG_NULLABLE _variable_declaration_part;
    plang_node_t PLANG_NULLABLE _subroutine_part;
};

PLANG_NODE_PARSE_PROTOTYPE(implementation_part);
PLANG_NODE_FREE_PROTOTYPE(implementation_part);

struct plang_node_subroutine_part {
    struct plang_node _header;

    plang_array_t _declarations;
};

PLANG_NODE_PARSE_PROTOTYPE(subroutine_part);
PLANG_NODE_FREE_PROTOTYPE(subroutine_part);


#if PLANG_CLASCAL
/* MARK: - Clascal */

PLANG_NODE_PARSE_PROTOTYPE(class_type_identifier);
/* No free, this is a meta-node. */

struct plang_node_forward_class_type {
    struct plang_node _header;

    /* Presence alone indicates type. */
};

PLANG_NODE_PARSE_PROTOTYPE(forward_class_type);
PLANG_NODE_FREE_PROTOTYPE(forward_class_type);

struct plang_node_class_type {
    struct plang_node _header;
    plang_node_t _declaration;        /*!< backwards link */

    plang_node_t PLANG_NULLABLE _superclass_type_identifier;
    plang_node_t PLANG_NULLABLE _field_list;
    plang_array_t PLANG_NULLABLE _method_interfaces;

    plang_scope_t _scope;
};

PLANG_NODE_PARSE_PROTOTYPE(class_type);
PLANG_NODE_FREE_PROTOTYPE(class_type);

struct plang_node_method_interface {
    struct plang_node _header;

    plang_node_t _heading;
    bool _is_abstract;
    bool _is_default;
    bool _is_override;
};

PLANG_NODE_PARSE_PROTOTYPE(method_interface);
PLANG_NODE_FREE_PROTOTYPE(method_interface);

struct plang_node_method_block {
    struct plang_node _header;

    plang_node_t _class_type_identifier;
    plang_node_t PLANG_NULLABLE _procedure_and_function_declaration_part;
    plang_node_t PLANG_NULLABLE _creation_block;

    plang_scope_t _scope;
};

PLANG_NODE_PARSE_PROTOTYPE(method_block);
PLANG_NODE_FREE_PROTOTYPE(method_block);

struct plang_node_creation_block {
    struct plang_node _header;

    plang_node_t _block;

    plang_scope_t _scope;
};

PLANG_NODE_PARSE_PROTOTYPE(creation_block);
PLANG_NODE_FREE_PROTOTYPE(creation_block);
#endif


PLANG_HEADER_END

#endif /* __plang_node_internal__h__ */
