/*
    test_tokenization.c
    plangTests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "test_tokenization.h"

#include <string.h>

#include "plang_token_internal.h"


PLANG_SOURCE_BEGIN


void
tokenization_setup(void)
{
    tests_shared_setup();

    /* Add other setup here. */
}


void
tokenization_teardown(void)
{
    /* Add other teardown here. */

    tests_shared_teardown();
}


START_TEST(tokenize_digit_sequence)
{
    const char *buf = "123";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t token = plang_next_token(parser);
    ck_assert_ptr_nonnull(token);

    ck_assert_int_eq(token->_type, plang_token_type_digit_sequence);
    ck_assert_int_eq(strncmp(plang_token_get_text(token), "123", 3), 0);
    ck_assert_int_eq(token->_range.length, 3);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_digit_sequence_with_leading_whitespace)
{
    const char *buf = "  \n123";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), "  \n", 3) == 0);
    ck_assert_int_eq(whitespace->_range.length, 3);

    plang_token_t digit_sequence = plang_next_token(parser);
    ck_assert_ptr_nonnull(digit_sequence);

    ck_assert_int_eq(digit_sequence->_type, plang_token_type_digit_sequence);
    ck_assert_int_eq(strncmp(plang_token_get_text(digit_sequence), "123", 3), 0);
    ck_assert_int_eq(digit_sequence->_range.length, 3);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(digit_sequence);
    plang_token_free(whitespace);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_digit_sequence_with_leading_and_trailing_whitespace)
{
    const char *buf = "  \n123\n ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace1 = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace1);

    ck_assert_int_eq(whitespace1->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace1), "  \n", 3) == 0);
    ck_assert_int_eq(whitespace1->_range.length, 3);

    plang_token_t digit_sequence = plang_next_token(parser);
    ck_assert_ptr_nonnull(digit_sequence);

    ck_assert_int_eq(digit_sequence->_type, plang_token_type_digit_sequence);
    ck_assert(strncmp(plang_token_get_text(digit_sequence), "123", 3) == 0);
    ck_assert_int_eq(digit_sequence->_range.length, 3);

    plang_token_t whitespace2 = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace2);

    ck_assert_int_eq(whitespace2->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace2), "\n ", 2) == 0);
    ck_assert_int_eq(whitespace2->_range.length, 2);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(digit_sequence);
    plang_token_free(whitespace1);
    plang_token_free(whitespace2);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_hex_literal)
{
    const char *buf = "$A9F4";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t token = plang_next_token(parser);
    ck_assert_ptr_nonnull(token);

    ck_assert_int_eq(token->_type, plang_token_type_hex_digit_sequence);
    ck_assert(strncmp(plang_token_get_text(token), "$A9F4", 5) == 0);
    ck_assert_int_eq(token->_range.length, 5);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(token);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_hex_then_integer_digit_sequence)
{
    const char *buf = "$A9F4 5678";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t hex_digit_sequence = plang_next_token(parser);
    ck_assert_ptr_nonnull(hex_digit_sequence);

    ck_assert_int_eq(hex_digit_sequence->_type, plang_token_type_hex_digit_sequence);
    ck_assert(strncmp(plang_token_get_text(hex_digit_sequence), "$A9F4", 5) == 0);
    ck_assert_int_eq(hex_digit_sequence->_range.length, 5);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), " ", 1) == 0);
    ck_assert_int_eq(whitespace->_range.length, 1);

    plang_token_t digit_sequence = plang_next_token(parser);
    ck_assert_ptr_nonnull(digit_sequence);

    ck_assert_int_eq(digit_sequence->_type, plang_token_type_digit_sequence);
    ck_assert(strncmp(plang_token_get_text(digit_sequence), "5678", 4) == 0);
    ck_assert_int_eq(digit_sequence->_range.length, 4);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(hex_digit_sequence);
    plang_token_free(whitespace);
    plang_token_free(digit_sequence);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_identifier)
{
    const char *buf = "Pascal";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t token = plang_next_token(parser);
    ck_assert_ptr_nonnull(token);

    ck_assert_int_eq(token->_type, plang_token_type_identifier);
    ck_assert(strncmp(plang_token_get_text(token), "Pascal", 6) == 0);
    ck_assert_int_eq(token->_range.length, 6);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(token);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_keyword)
{
    const char *buf = "   UNIT  ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    ck_assert_int_eq(whitespace->_range.length, 3);

    plang_token_t unit = plang_next_token(parser);
    ck_assert_ptr_nonnull(unit);

    ck_assert_int_eq(unit->_type, plang_token_type_UNIT);
    ck_assert(strncmp(plang_token_get_text(unit), "UNIT", 4) == 0);
    ck_assert_int_eq(unit->_range.length, 4);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(unit);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_string_literal)
{
    const char *buf = "  'The String''s Literal' ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), "  ", 2) == 0);
    ck_assert_int_eq(whitespace->_range.length, 2);

    plang_token_t quoted_string = plang_next_token(parser);
    ck_assert_ptr_nonnull(quoted_string);

    ck_assert_int_eq(quoted_string->_type, plang_token_type_string_literal);
    ck_assert(strncmp(plang_token_get_text(quoted_string), "'The String''s Literal'", 23) == 0);
    ck_assert_int_eq(quoted_string->_range.length, 23);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(quoted_string);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_brace_comment)
{
    const char *buf = "   { comment text () }   ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    ck_assert_int_eq(whitespace->_range.length, 3);

    plang_token_t comment = plang_next_token(parser);
    ck_assert_ptr_nonnull(comment);

    ck_assert_int_eq(comment->_type, plang_token_type_brace_comment);
    ck_assert(strncmp(plang_token_get_text(comment), "{ comment text () }", 19) == 0);
    ck_assert_int_eq(comment->_range.length, 19);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(comment);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_parentheses_comment)
{
    const char *buf = "   (* comment text () *)   ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    ck_assert_ptr_nonnull(whitespace);

    ck_assert_int_eq(whitespace->_type, plang_token_type_whitespace);
    ck_assert(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    ck_assert_int_eq(whitespace->_range.length, 3);

    plang_token_t comment = plang_next_token(parser);
    ck_assert_ptr_nonnull(comment);

    ck_assert_int_eq(comment->_type, plang_token_type_parentheses_comment);
    ck_assert(strncmp(plang_token_get_text(comment), "(* comment text () *)", 21) == 0);
    ck_assert_int_eq(comment->_range.length, 21);

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_token_free(comment);
    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_one_character_special_symbols)
{
    const char *buf = "  + - * / = < > [ ] . , ( ) : ; ^ @ ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    struct expected_value {
        const char *_text;
        const size_t _text_len;
        const plang_token_type_t _type;
    } expected_values[] = {
        { "+", 1, plang_token_type_PLUS },
        { "-", 1, plang_token_type_MINUS },
        { "*", 1, plang_token_type_ASTERISK },
        { "/", 1, plang_token_type_SLASH },
        { "=", 1, plang_token_type_EQUALS },
        { "<", 1, plang_token_type_LT },
        { ">", 1, plang_token_type_GT },
        { "[", 1, plang_token_type_LBRACKET },
        { "]", 1, plang_token_type_RBRACKET },
        { ".", 1, plang_token_type_PERIOD },
        { ",", 1, plang_token_type_COMMA },
        { "(", 1, plang_token_type_LPAREN },
        { ")", 1, plang_token_type_RPAREN },
        { ":", 1, plang_token_type_COLON },
        { ";", 1, plang_token_type_SEMICOLON },
        { "^", 1, plang_token_type_CIRCUMFLEX },
        { "@", 1, plang_token_type_AT },
        { NULL, 0, plang_token_type_unknown },
    };

    for (struct expected_value *expected = &expected_values[0];
         expected->_text != NULL;
         expected++)
    {
        /* Skip whitespace used for test readability. */
        plang_token_t token = NULL;
        do {
            token = plang_next_token(parser);
        } while (plang_token_matches(token, plang_token_type_whitespace));

        ck_assert_ptr_nonnull(token);

        ck_assert_int_eq(token->_type, expected->_type);
        ck_assert(strncmp(plang_token_get_text(token), expected->_text, 1) == 0);
        ck_assert_int_eq(token->_range.length, expected->_text_len);

        plang_token_free(token);
    }

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST

START_TEST(tokenize_two_character_special_symbols)
{
    const char *buf = "<> <= >= := ..";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    ck_assert_ptr_nonnull(source);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    ck_assert_ptr_nonnull(sources);
    plang_parser_t parser = plang_parser_new(sources, tests_log);
    ck_assert_ptr_nonnull(parser);

    bool parser_setup = plang_parser_setup(parser);
    ck_assert(parser_setup);

    struct expected_value {
        const char *_text;
        const size_t _text_len;
        const plang_token_type_t _type;
    } expected_values[] = {
        { "<>", 2, plang_token_type_NOT_EQUAL },
        { "<=", 2, plang_token_type_LTE },
        { ">=", 2, plang_token_type_GTE },
        { ":=", 2, plang_token_type_ASSIGNMENT },
        { "..", 2, plang_token_type_RANGE },
        { NULL, 0, plang_token_type_unknown },
    };

    for (struct expected_value *expected = &expected_values[0];
         expected->_text != NULL;
         expected++)
    {
        /* Skip whitespace used for test readability. */
        plang_token_t token = NULL;
        do {
            token = plang_next_token(parser);
        } while (plang_token_matches(token, plang_token_type_whitespace));

        ck_assert_ptr_nonnull(token);

        ck_assert_int_eq(token->_type, expected->_type);
        ck_assert_int_eq(strncmp(plang_token_get_text(token), expected->_text, 2), 0);
        ck_assert_int_eq(token->_range.length, expected->_text_len);

        plang_token_free(token);
    }

    bool parser_teardown = plang_parser_teardown(parser);
    ck_assert(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *
tokenization_suite(void)
{
    Suite *s = suite_create("Tokenization");

    TCase *tc_tokenization = tcase_create("Tokenization");
    tcase_add_checked_fixture(tc_tokenization,
                              tokenization_setup,
                              tokenization_teardown);
    tcase_add_test(tc_tokenization, tokenize_digit_sequence);
    tcase_add_test(tc_tokenization, tokenize_digit_sequence_with_leading_whitespace);
    tcase_add_test(tc_tokenization, tokenize_digit_sequence_with_leading_and_trailing_whitespace);
    tcase_add_test(tc_tokenization, tokenize_hex_literal);
    tcase_add_test(tc_tokenization, tokenize_hex_then_integer_digit_sequence);
    tcase_add_test(tc_tokenization, tokenize_identifier);
    tcase_add_test(tc_tokenization, tokenize_keyword);
    tcase_add_test(tc_tokenization, tokenize_string_literal);
    tcase_add_test(tc_tokenization, tokenize_brace_comment);
    tcase_add_test(tc_tokenization, tokenize_parentheses_comment);
    tcase_add_test(tc_tokenization, tokenize_one_character_special_symbols);
    tcase_add_test(tc_tokenization, tokenize_two_character_special_symbols);
    suite_add_tcase(s, tc_tokenization);

    return s;
}


PLANG_SOURCE_END
