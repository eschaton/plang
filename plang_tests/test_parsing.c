/*
    test_parsing.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "test_parsing.h"

#include "plang_parser_internal.h"

#include "plang_node_internal.h"
#include "plang_procedure.h"
#include "plang_scope.h"
#include "plang_token.h"


PLANG_SOURCE_BEGIN


void
parsing_setup(void)
{
    tests_shared_setup();

    /* Add other setup here. */
}


void
parsing_teardown(void)
{
    /* Add other teardown here. */

    tests_shared_teardown();
}


START_TEST(parse_trivial_unit)
{
    char *buf = NULL;
    size_t buf_len = 0;
    bool read_file = tests_read_file("TrivialUnit.pas", &buf, &buf_len);
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

START_TEST(parse_preamble_unit)
{
    char *buf = NULL;
    size_t buf_len = 0;
    bool read_file = tests_read_file("PreambleUnit.pas", &buf, &buf_len);
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

START_TEST(parse_trivial_program)
{
    char *buf = NULL;
    size_t buf_len = 0;
    bool read_file = tests_read_file("TrivialProgram.pas", &buf, &buf_len);
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

START_TEST(parse_Base64_unit_with_preamble)
{
    char *preamble_buf = NULL;
    size_t preamble_buf_len = 0;
    bool read_preamble = tests_read_file("PreambleUnit.pas",
                                         &preamble_buf,
                                         &preamble_buf_len);
    ck_assert_msg(read_preamble, "Failed to read PreambleUnit.pas");
    ck_assert_uint_ge(preamble_buf_len, 0);

    plang_source_t preamble_source
        = plang_source_new("PreambleUnit.pas",
                           preamble_buf,
                           preamble_buf_len);
    ck_assert_ptr_nonnull(preamble_source);

    char *base64_buf = NULL;
    size_t base64_buf_len = 0;
    bool read_base64 = tests_read_file("Base64.pas",
                                       &base64_buf,
                                       &base64_buf_len);
    ck_assert_msg(read_base64, "Failed to read Base64.pas");
    ck_assert_uint_ge(base64_buf_len, 0);

    plang_source_t base64_source = plang_source_new("Base64.pas",
                                                    base64_buf,
                                                    base64_buf_len);
    ck_assert_ptr_nonnull(base64_source);

    plang_array_t sources = plang_array_new_with_items(base64_source,
                                                       NULL);
    ck_assert_ptr_nonnull(sources);

    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    plang_parser_set_preamble(parser, preamble_source);

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
    plang_source_free(preamble_source);
}

START_TEST(parse_real_number)
{
    const char *buf =
    "PROGRAM RealNum;" "\n"
    "" "\n"
    "CONST" "\n"
    "    pi = 3.14159;" "\n"
    "    mol = 6.022e+23;" "\n"
    "" "\n"
    "PROCEDURE DoNothing; EXTERNAL;" "\n"
    "" "\n"
    "BEGIN" "\n"
    "    DoNothing" "\n"
    "END." "\n";
    const size_t buf_len = strlen(buf);

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
parsing_suite(void)
{
    Suite *s = suite_create("Parsing");

    TCase *tc_parsing = tcase_create("Parsing");
    tcase_add_checked_fixture(tc_parsing,
                              parsing_setup,
                              parsing_teardown);
    tcase_add_test(tc_parsing, parse_trivial_unit);
    tcase_add_test(tc_parsing, parse_preamble_unit);
    tcase_add_test(tc_parsing, parse_trivial_program);
    tcase_add_test(tc_parsing, parse_Base64_unit_with_preamble);
    tcase_add_test(tc_parsing, parse_real_number);
    suite_add_tcase(s, tc_parsing);

    return s;
}


PLANG_SOURCE_END
