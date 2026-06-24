/*
    tests_support.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "tests_support.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_tokenization.h"


PLANG_SOURCE_BEGIN


plang_array_t PLANG_NULLABLE tests_log_array = NULL;
plang_log_t PLANG_NULLABLE tests_log = NULL;


const char * const tests_log_level_name(const plang_log_level_t level)
{
    switch (level) {
        case plang_log_level_debug:   return "debug";
        case plang_log_level_info:    return "info";
        case plang_log_level_notice:  return "notice";
        case plang_log_level_warning: return "warning";
        case plang_log_level_error:   return "error";
    }
}


void tests_log_output(const plang_log_level_t level,
                      const char *message,
                      int indent,
                      void *context)
{
    plang_array_t log_array = context;
    assert(log_array != NULL);

    const char * const level_name = tests_log_level_name(level);
    const size_t level_name_len = 9; /* maximum, plus colon & space */
    const size_t entry_len = level_name_len + indent + strlen(message);
    char *entry = calloc(sizeof(char), entry_len + 1);
    assert(entry != NULL);

    snprintf(entry, entry_len+1, "%s: %s", level_name, message);

    bool appended = plang_array_append(log_array, entry);
    assert(appended != false);
}


void tests_shared_setup(void)
{
    /* Set up a log for the tests, to put the messages in an array. */
    {
        tests_log_array = plang_array_new(8);
        assert(tests_log_array != NULL);

        tests_log = plang_log_new(tests_log_output, tests_log_array);
        assert(tests_log != NULL);
    }
}


void tests_shared_teardown(void)
{
    /* Tear down the test log. */
    {
        plang_log_free(tests_log); tests_log = NULL;

        const size_t count = plang_array_get_count(tests_log_array);
        for (size_t i = 0; i < count; i++) {
            char *entry = plang_array_get_item(tests_log_array, i);
            free(entry);
        }
        plang_array_free(tests_log_array); tests_log_array = NULL;
    }
}


bool
tests_read_file(const char *name,
                char * PLANG_NULLABLE * PLANG_NONNULL data,
                size_t * PLANG_NONNULL data_len)
{
    char fullpath[PATH_MAX] = {0};
    snprintf(fullpath, PATH_MAX, "plang_tests/%s", name);

    FILE *f = fopen(fullpath, "r");
    if (f) {
        int seek_err = fseeko(f, 0, SEEK_END);
        if (seek_err != -1) {
            off_t f_len = ftello(f);
            if (f_len != -1) {
                int reset_err = fseeko(f, 0, SEEK_SET);
                if (reset_err != -1) {
                    char *buf = calloc(f_len, sizeof(char));
                    if (buf != NULL) {
                        ssize_t items = fread(buf, f_len, 1, f);
                        if (items == 1) {
                            *data = buf;
                            *data_len = f_len;
                            return true;
                        } else {
                            free(buf);
                            buf = NULL;
                        }
                    }
                }
            }
        }

        fclose(f);
    }

    return false;
}


PLANG_SOURCE_END
