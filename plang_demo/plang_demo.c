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
            fprintf(stderr,
                    "plang: error: 0: failed to read preamble: %s" "\n",
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

    parser = plang_driver(preamble, sources, NULL);

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
plang_scope_describe(plang_scope_t scope)
{
    plang_dictionary_t constants = plang_scope_copy_constants(scope);
    if (constants) {
        fprintf(stdout, "\t" "Constants: %zd" "\n",
                plang_dictionary_get_count(constants));
        plang_dictionary_free(constants);
    }

    plang_dictionary_t types = plang_scope_copy_types(scope);
    if (types) {
        fprintf(stdout, "\t" "Types: %zd" "\n",
                plang_dictionary_get_count(types));
        plang_dictionary_free(types);
    }

    plang_dictionary_t variables = plang_scope_copy_variables(scope);
    if (variables) {
        fprintf(stdout, "\t" "Variables: %zd" "\n",
                plang_dictionary_get_count(variables));
        plang_dictionary_free(variables);
    }

    plang_dictionary_t procedures = plang_scope_copy_procedures(scope);
    if (procedures) {
        fprintf(stdout, "\t" "Procedures: %zd" "\n",
                plang_dictionary_get_count(procedures));
        plang_dictionary_free(procedures);
    }

    plang_dictionary_t functions = plang_scope_copy_functions(scope);
    if (functions) {
        fprintf(stdout, "\t" "Functions: %zd" "\n",
                plang_dictionary_get_count(functions));
        plang_dictionary_free(functions);
    }
}


const char * PLANG_NULLABLE
plang_program_copy_name(plang_program_t program)
{
    plang_token_t identifier = plang_program_get_identifier(program);

    return plang_token_copy_text(identifier);
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

    fprintf(stdout, "Unit '%s' {" "\n", name);
    free((void *) name);

    fprintf(stdout, "Interface: {" "\n");
    plang_scope_t interface_scope
        = plang_unit_get_interface_scope(unit);
    plang_scope_describe(interface_scope);
    fprintf(stdout, "}" "\n");

    fprintf(stdout, "Implementation: {" "\n");
    plang_scope_t implementation_scope
        = plang_unit_get_implementation_scope(unit);
    plang_scope_describe(implementation_scope);
    fprintf(stdout, "}" "\n");

    fprintf(stdout, "}" "\n");
}


void
plang_program_describe(plang_program_t program)
{
    const char *name = plang_program_copy_name(program);

    fprintf(stdout, "Program '%s' {" "\n", name);
    free((void *) name);

    plang_scope_t program_scope = plang_program_get_scope(program);
    plang_scope_describe(program_scope);

    fprintf(stdout, "}" "\n");
}


int
main(int argc,
     const char * PLANG_NULLABLE argv[])
{
    if (argc < 3) {
        fprintf(stderr, "plang: error: 0: insufficient arguments" "\n");
        return EX_USAGE;
    }

    /* Pass an empty second argument to skip the preamble. */

    const char *preamble_path = (strlen(argv[1]) > 0) ? argv[1] : NULL;
    const char **source_paths = &argv[2];
    plang_parser_t parser = plang_raw_driver(preamble_path,
                                             source_paths,
                                             argc - 2);
    if (parser == NULL) {
        fprintf(stderr, "plang: error: 0: error in parser" "\n");
        return EX_SOFTWARE;
    }

    /* Print some information about what was parsed. */

    fprintf(stdout, "plang: Parse successful (enough)" "\n");

    plang_unit_t preamble = plang_parser_get_preamble_unit(parser);
    if (preamble) {
        fprintf(stdout, "plang: Parsed preamble" "\n");

        plang_unit_describe(preamble);
    }

    plang_dictionary_t units_dict = plang_parser_copy_all_units(parser);
    if (units_dict) {
        fprintf(stdout, "\n");

        plang_array_t units
            = plang_dictionary_copy_all_values(units_dict);

        const size_t count = plang_array_get_count(units);
        fprintf(stdout, "plang: Parsed %zd unit(s)" "\n", count);

        for (size_t i = 0; i < count; i++) {
            plang_unit_t unit = plang_array_get_item(units, i);

            plang_unit_describe(unit);
        }

        plang_array_free(units);
        plang_dictionary_free(units_dict);
    }

    plang_program_t program = plang_parser_get_program(parser);
    if (program) {
        fprintf(stdout, "\n");

        fprintf(stdout, "Plang: Parsed program" "\n");

        plang_program_describe(program);

    }

    return EX_OK;
}


PLANG_SOURCE_END
