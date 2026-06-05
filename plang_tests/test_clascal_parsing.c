/*
    test_clascal_parsing.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "test_clascal_parsing.h"


PLANG_SOURCE_BEGIN


#if PLANG_CLASCAL


void
clascal_parsing_setup(void)
{
    tests_shared_setup();

    /* Add other setup here. */
}


void
clascal_parsing_teardown(void)
{
    /* Add other teardown here. */

    tests_shared_teardown();
}


START_TEST(parse_trivial_clascal_unit)
{
    char *buf = NULL;
    size_t buf_len = 0;
    bool read_file = tests_read_file("TrivialClascal.pas", &buf, &buf_len);
    ck_assert_msg(read_file, "Failed to read file");
    ck_assert_uint_ge(buf_len, 0);

    plang_source_t source = plang_source_new(NULL, buf, buf_len);
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parsed = plang_parser_run(parser);
    plang_array_t errors = plang_parser_copy_errors(parser);
    const size_t errors_count = errors ? plang_array_get_count(errors) : 0;
    ck_assert_msg(parsed, "Parse failed, %zu errors", errors_count);

    for (size_t error_idx = 0; error_idx < errors_count; error_idx++) {
        plang_error_t error = plang_array_get_item(errors, error_idx);
        ck_assert_ptr_nonnull(error);
        const char *error_text = plang_error_copy_text(error);
        ck_assert_ptr_nonnull(error_text);
        ck_assert_msg(false, "%zu: %s", error_idx, error_text);
        free((void *)error_text);
    }

    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *
clascal_parsing_suite(void)
{
    Suite *s = suite_create("ClascalParsing");

    TCase *tc_clascal_parsing = tcase_create("ClascalParsing");
    tcase_add_checked_fixture(tc_clascal_parsing,
                              clascal_parsing_setup,
                              clascal_parsing_teardown);
    tcase_add_test(tc_clascal_parsing, parse_trivial_clascal_unit);
    suite_add_tcase(s, tc_clascal_parsing);

    return s;
}


#endif


PLANG_SOURCE_END
