/*
    plang_demo.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang.h"

#include <stdio.h>
#include <string.h>
#include <sysexits.h>


PLANG_SOURCE_BEGIN


/* Forward Declarations */

void
plang_constant_decribe(plang_constant_t constant);

void
plang_type_describe(plang_type_t type);

void
plang_variable_describe(plang_variable_t variable,
                        plang_scope_t scope);

void
plang_procedure_describe(plang_procedure_t procedure);

void
plang_function_describe(plang_function_t function,
                        plang_scope_t scope);

void
plang_scope_describe(plang_scope_t scope);

const char * PLANG_NULLABLE
plang_unit_copy_name(plang_unit_t unit);

void
plang_unit_describe(plang_unit_t unit);

const char * PLANG_NULLABLE
plang_program_copy_name(plang_program_t program);

void
plang_program_describe(plang_program_t program);


/*! The log used by the demo for its output. */
static plang_log_t shared_log = NULL;


void
plang_demo_output(plang_log_level_t level,
                  const char *message,
                  int indent,
                  void * PLANG_NULLABLE context)
{
    /* Write warnings and above to stderr, all else to stdout. */

    FILE *output = (level >= plang_log_level_warning) ? stderr : stdout;

    /* Include the type of log in the output. */

    const char *type;
    switch (level) {
        case plang_log_level_debug:   type = "debug";   break;
        case plang_log_level_info:    type = "info";    break;
        case plang_log_level_notice:  type = "notice";  break;
        case plang_log_level_warning: type = "warning"; break;
        case plang_log_level_error:   type = "error";   break;
    }

    static char *indentation_template
        = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
    int real_indent = (indent > 16) ? 16 : indent;
    if (real_indent < 0) real_indent = 0;
    char *indentation = &indentation_template[16 - real_indent];

    fprintf(output, "plang: %s: %s%s" "\n", type, indentation, message);
    fflush(output);
}


plang_parser_t PLANG_NULLABLE
plang_raw_driver(const char * PLANG_NULLABLE preamble_path,
                 const char * PLANG_NULLABLE source_paths[],
                 const size_t source_paths_count)
{
    plang_parser_t parser = NULL;
    plang_source_t preamble = NULL;
    plang_array_t sources = NULL;

    if (preamble_path != NULL) {
        preamble = plang_source_new_from_file(preamble_path);
        if (preamble == NULL) {
            plang_log(shared_log, plang_log_level_error,
                      "failed to read preamble: %s",
                      preamble_path);
            goto error;
        }
    }

    sources = plang_array_new(source_paths_count);
    if (sources == NULL) {
        goto error;
    }

    for (size_t i = 0; i < source_paths_count; i++) {
        const char *source_path = source_paths[i];
        plang_source_t source = plang_source_new_from_file(source_path);
        if (source == NULL) {
            goto error;
        }

        bool appended = plang_array_append(sources, source);
        if (appended == false) {
            goto error;
        }
    }

    parser = plang_driver(preamble, sources, shared_log);

    return parser;

error:
    plang_source_free(preamble);
    if (sources) {
        const size_t count = plang_array_get_count(sources);
        for (size_t i = 0; i < count; i++) {
            plang_source_t source = plang_array_get_item(sources, i);
            plang_source_free(source);
        }
        plang_array_free(sources);
    }
    plang_parser_free(parser);

    return NULL;
}


void
plang_constant_describe(plang_constant_t constant)
{
    plang_token_t identifier = plang_constant_get_identifier(constant);
    const char *constant_name = plang_token_copy_text(identifier);

    plang_log(shared_log, plang_log_level_info, "'%s';", constant_name);

    free((void *) constant_name);
}


void
plang_type_describe(plang_type_t type)
{
    plang_token_t identifier = plang_type_get_identifier(type);
    const char *type_name = plang_token_copy_text(identifier);

    plang_log(shared_log, plang_log_level_info, "'%s';", type_name);

    free((void *) type_name);
}


void
plang_variable_describe(plang_variable_t variable,
                        plang_scope_t scope)
{
    plang_token_t identifier = plang_variable_get_identifier(variable);
    const char *variable_name = plang_token_copy_text(identifier);

    plang_type_t type = plang_variable_get_type(variable, scope);
    plang_token_t type_identifier = plang_type_get_identifier(type);
    const char *type_name = plang_token_copy_text(type_identifier);

    plang_log(shared_log, plang_log_level_info,
              "%s: '%s';", variable_name, type_name);

    free((void *) variable_name);
    free((void *) type_name);
}


void
plang_procedure_describe(plang_procedure_t procedure)
{
    plang_token_t identifier
        = plang_procedure_get_identifier(procedure);
    const char *procedure_name = plang_token_copy_text(identifier);

    plang_log_indent(shared_log, plang_log_level_info,
                     "PROCEDURE '%s' {", procedure_name);
    plang_scope_t procedure_scope
        = plang_procedure_get_scope(procedure);
    plang_scope_describe(procedure_scope);
    plang_log_outdent(shared_log, plang_log_level_info, "}");

    free((void *) procedure_name);
}


void
plang_function_describe(plang_function_t function,
                        plang_scope_t scope)
{
    plang_token_t identifier
        = plang_function_get_identifier(function);
    const char *function_name = plang_token_copy_text(identifier);

    plang_type_t result_type = plang_function_get_result_type(function,
                                                              scope);
    plang_token_t result_type_identifier
        = plang_type_get_identifier(result_type);
    const char *result_type_name
        = plang_token_copy_text(result_type_identifier);

    plang_log_indent(shared_log, plang_log_level_info,
                     "FUNCTION '%s': '%s' {", function_name,
                     result_type_name);
    plang_scope_t function_scope = plang_function_get_scope(function);
    plang_scope_describe(function_scope);
    plang_log_outdent(shared_log, plang_log_level_info, "}");

    free((void *) function_name);
    free((void *) result_type_name);
}


void
plang_scope_describe(plang_scope_t scope)
{
    plang_dictionary_t constants = plang_scope_copy_constants(scope);
    if (constants) {
        const size_t count = plang_dictionary_get_count(constants);
        if (count > 0) {
            plang_log_indent(shared_log, plang_log_level_info,
                             "Constants (%zd): {",
                             plang_dictionary_get_count(constants));
            plang_array_t all_constants
                = plang_dictionary_copy_all_values(constants);
            for (size_t i = 0; i < count; i++) {
                plang_constant_t constant
                    = plang_array_get_item(all_constants, i);
                plang_constant_describe(constant);
            }
            plang_array_free(all_constants);
            plang_log_outdent(shared_log, plang_log_level_info, "}");
        } else {
            plang_log(shared_log, plang_log_level_info,
                      "Constants (0);");
        }
        plang_dictionary_free(constants);
    }

    plang_dictionary_t types = plang_scope_copy_types(scope);
    if (types) {
        const size_t count = plang_dictionary_get_count(types);
        if (count > 0) {
            plang_log_indent(shared_log, plang_log_level_info,
                             "Types (%zd): {",
                             plang_dictionary_get_count(types));
            plang_array_t all_types
                = plang_dictionary_copy_all_values(types);
            for (size_t i = 0; i < count; i++) {
                plang_type_t type = plang_array_get_item(all_types, i);
                plang_type_describe(type);
            }
            plang_array_free(all_types);
            plang_log_outdent(shared_log, plang_log_level_info, "}");
        } else {
            plang_log(shared_log, plang_log_level_info, "Types (0);");
        }
        plang_dictionary_free(types);
    }

    plang_dictionary_t variables = plang_scope_copy_variables(scope);
    if (variables) {
        const size_t count = plang_dictionary_get_count(variables);
        if (count > 0) {
            plang_log_indent(shared_log, plang_log_level_info,
                             "Variables (%zd): {",
                             plang_dictionary_get_count(variables));
            plang_array_t all_variables
                = plang_dictionary_copy_all_values(variables);
            for (size_t i = 0; i < count; i++) {
                plang_variable_t variable
                    = plang_array_get_item(all_variables, i);
                plang_variable_describe(variable, scope);
            }
            plang_array_free(all_variables);
            plang_log_outdent(shared_log, plang_log_level_info, "}");
        } else {
            plang_log(shared_log, plang_log_level_info,
                      "Variables (0);");
        }
        plang_dictionary_free(variables);
    }

    plang_dictionary_t procedures = plang_scope_copy_procedures(scope);
    if (procedures) {
        const size_t count = plang_dictionary_get_count(procedures);
        if (count > 0) {
            plang_log_indent(shared_log, plang_log_level_info,
                             "Procedures (%zd): {",
                             plang_dictionary_get_count(procedures));
            plang_array_t all_procedures
                = plang_dictionary_copy_all_values(procedures);
            for (size_t i = 0; i < count; i++) {
                plang_procedure_t procedure
                    = plang_array_get_item(all_procedures, i);
                plang_procedure_describe(procedure);
            }
            plang_array_free(all_procedures);
            plang_log_outdent(shared_log, plang_log_level_info, "}");
        } else {
            plang_log(shared_log, plang_log_level_info,
                      "Procedures (0);");
        }
        plang_dictionary_free(procedures);
    }

    plang_dictionary_t functions = plang_scope_copy_functions(scope);
    if (functions) {
        const size_t count = plang_dictionary_get_count(functions);
        if (count > 0) {
            plang_log_indent(shared_log, plang_log_level_info,
                             "Functions (%zd): {",
                             plang_dictionary_get_count(functions));
            plang_array_t all_functions
                = plang_dictionary_copy_all_values(functions);
            for (size_t i = 0; i < count; i++) {
                plang_function_t function
                    = plang_array_get_item(all_functions, i);
                plang_function_describe(function, scope);
            }
            plang_array_free(all_functions);
            plang_log_outdent(shared_log, plang_log_level_info, "}");
        } else {
            plang_log(shared_log, plang_log_level_info,
                      "Functions (0);");
        }
        plang_dictionary_free(functions);
    }
}


const char * PLANG_NULLABLE
plang_unit_copy_name(plang_unit_t unit)
{
    plang_token_t identifier = plang_unit_get_identifier(unit);

    return plang_token_copy_text(identifier);
}


void
plang_unit_describe(plang_unit_t unit)
{
    const char *name = plang_unit_copy_name(unit);
    plang_log_indent(shared_log, plang_log_level_info, "Unit '%s' {",
                     name);
    free((void *) name);

    plang_log_indent(shared_log, plang_log_level_info, "Interface: {");
    plang_scope_t interface_scope
        = plang_unit_get_interface_scope(unit);
    plang_scope_describe(interface_scope);
    plang_log_outdent(shared_log, plang_log_level_info, "}");

    plang_log_indent(shared_log, plang_log_level_info,
                     "Implementation: {");
    plang_scope_t implementation_scope
        = plang_unit_get_implementation_scope(unit);
    plang_scope_describe(implementation_scope);
    plang_log_outdent(shared_log, plang_log_level_info, "}");

    plang_log_outdent(shared_log, plang_log_level_info, "}");
}


const char * PLANG_NULLABLE
plang_program_copy_name(plang_program_t program)
{
    plang_token_t identifier = plang_program_get_identifier(program);

    return plang_token_copy_text(identifier);
}


void
plang_program_describe(plang_program_t program)
{
    const char *name = plang_program_copy_name(program);

    plang_log_indent(shared_log, plang_log_level_info,
                     "Program '%s' {", name);
    free((void *) name);

    plang_scope_t program_scope = plang_program_get_scope(program);
    plang_scope_describe(program_scope);

    plang_log_outdent(shared_log, plang_log_level_info,
                      "}", name);
}


int
main(int argc,
     const char * PLANG_NULLABLE argv[])
{
    if (argc < 3) {
        fprintf(stderr, "plang: error: insufficient arguments" "\n");
        return EX_USAGE;
    }

    /* Create the shared log and set it to log everything. */

    shared_log = plang_log_new(plang_demo_output, NULL);
    if (shared_log == NULL) {
        fprintf(stderr,
                "plang: error: failed to create output log" "\n");
        return EX_OSERR;
    }

    plang_log_set_default_level(shared_log, plang_log_level_debug);

    /* Pass an empty second argument to skip the preamble. */

    const char *preamble_path = (strlen(argv[1]) > 0) ? argv[1] : NULL;
    const char **source_paths = &argv[2];
    plang_parser_t parser = plang_raw_driver(preamble_path,
                                             source_paths,
                                             argc - 2);
    if (parser == NULL) {
        plang_log(shared_log, plang_log_level_error,
                  "error in parser");
        return EX_SOFTWARE;
    }

    /* Print some information about what was parsed. */

    plang_log(shared_log, plang_log_level_info,
              "Parse successful (enough)");

    plang_unit_t preamble = plang_parser_get_preamble_unit(parser);
    if (preamble) {
        plang_log(shared_log, plang_log_level_info,
                  "Parsed preamble");

        plang_unit_describe(preamble);
    }

    plang_dictionary_t units_dict = plang_parser_copy_all_units(parser);
    if (units_dict) {
        plang_array_t units
            = plang_dictionary_copy_all_values(units_dict);

        const size_t count = plang_array_get_count(units);
        plang_log(shared_log, plang_log_level_info,
                  "Parsed %zd unit(s)", count);

        for (size_t i = 0; i < count; i++) {
            plang_unit_t unit = plang_array_get_item(units, i);

            plang_unit_describe(unit);
        }

        plang_array_free(units);
        plang_dictionary_free(units_dict);
    }

    plang_program_t program = plang_parser_get_program(parser);
    if (program) {
        plang_program_describe(program);

    }

    plang_log_free(shared_log);
    shared_log = NULL;

    return EX_OK;
}


PLANG_SOURCE_END
