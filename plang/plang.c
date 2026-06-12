/*
    plang.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang.h"

#include <limits.h>
#include <stdio.h>
#include <sysexits.h>


PLANG_SOURCE_BEGIN


void plang_driver_output_default(const char *message)
{
    fprintf(stderr, "%s" "\n", message);
}


bool
plang_driver(plang_source_t PLANG_NULLABLE preamble,
             plang_array_t sources,
             plang_driver_output_t PLANG_NULLABLE error_output)
{
    plang_array_t errors = NULL;
    plang_driver_output_t output = NULL;

    output = ((error_output != NULL)
              ? error_output
              : plang_driver_output_default);

    plang_parser_t parser = plang_parser_new(sources);
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

        plang_parser_free(parser);
    }

    return (errors == NULL);
}


int
plang_raw_driver(const char * PLANG_NULLABLE preamble_path,
                 const char * PLANG_NULLABLE source_paths[],
                 const size_t source_paths_count)
{
    int result = EX_OK;

    plang_source_t preamble = NULL;
    plang_array_t sources = NULL;
    plang_parser_t parser = NULL;
    plang_array_t errors = NULL;

    if (preamble_path != NULL) {
        preamble = plang_source_new_from_file(preamble_path);
        if (preamble == NULL) {
            fprintf(stderr,
                    "plang: error: 0: failed to read preamble: %s" "\n",
                    preamble_path);
            result = EX_IOERR;
            goto done;
        }
    }

    sources = plang_array_new(source_paths_count);
    if (sources == NULL) {
        result = EX_SOFTWARE;
        goto done;
    }

    for (size_t i = 0; i < source_paths_count; i++) {
        const char *source_path = source_paths[i];
        plang_source_t source = plang_source_new_from_file(source_path);
        if (source == NULL) {
            result = EX_SOFTWARE;
            goto done;
        }

        bool appended = plang_array_append(sources, source);
        if (appended == false) {
            result = EX_SOFTWARE;
            goto done;
        }
    }

    bool success = plang_driver(preamble, sources, NULL);
    if (!success) {
        result = EX_DATAERR;
    }

done:
    plang_array_free(errors);
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

    return result;
}


#if PLANG_MAIN
int
main(int argc,
     const char * PLANG_NULLABLE argv[])
{
    if (argc < 3) {
        fprintf(stderr, "plang: error: 0: insufficient arguments" "\n");
        return EX_USAGE;
    }

    const char *preamble_path = argv[1];
    const char **source_paths = &argv[2];

    return plang_raw_driver(preamble_path, source_paths, argc - 2);
}
#endif


PLANG_SOURCE_END
