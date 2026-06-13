/*
    plang.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang.h"

#include <limits.h>
#include <stdio.h>


PLANG_SOURCE_BEGIN


void plang_driver_output_default(const char *message)
{
    fprintf(stderr, "%s" "\n", message);
}


plang_parser_t PLANG_NULLABLE
plang_driver(plang_source_t PLANG_NULLABLE preamble,
             plang_array_t sources,
             plang_driver_output_t PLANG_NULLABLE error_output)
{
    plang_parser_t parser = NULL;
    plang_array_t errors = NULL;
    plang_driver_output_t output = NULL;

    output = ((error_output != NULL)
              ? error_output
              : plang_driver_output_default);

    parser = plang_parser_new(sources);
    if (parser != NULL) {
        if (preamble != NULL) {
            plang_parser_set_preamble(parser, preamble);
        }

        (void) plang_parser_run(parser);

        errors = plang_parser_copy_errors(parser);
        if (errors != NULL) {
            const size_t errors_count = plang_array_get_count(errors);
            for (size_t i = 0; i < errors_count; i++) {
                char message[PATH_MAX];
                plang_error_t error = plang_array_get_item(errors, i);
                plang_range_t range = plang_error_get_range(error);
                const char *raw_message = plang_error_copy_text(error);
                if (raw_message) {
                    snprintf(message, PATH_MAX,
                             "plang: error: %zu:%zu: %s",
                             range.start, range.start + range.length,
                             raw_message);
                    free((void *)raw_message);
                } else {
                    snprintf(message, PATH_MAX,
                             "plang: error: %zu:%zu: unknown",
                             range.start, range.start + range.length);
                }
                (*output)(message);
            }
        }

        plang_array_free(errors);
    }

    return parser;
}


PLANG_SOURCE_END
