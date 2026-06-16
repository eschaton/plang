/*
    plangTestTokenization.m
    plang Tests

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#import "plangTestCase.h"

#include "plang_token_internal.h"


NS_ASSUME_NONNULL_BEGIN


@interface plangTestTokenization : plangTestCase
@end


@implementation plangTestTokenization

- (void)testTokenizeDigitSequence
{
    const char *buf = "123";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t token = plang_next_token(parser);
    XCTAssertNotEqual(token, NULL);

    XCTAssertEqual(token->_type, plang_token_type_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(token), "123", 3) == 0);
    XCTAssertEqual(token->_range.length, 3);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeDigitSequenceWithLeadingWhitespace
{
    const char *buf = "  \n123";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), "  \n", 3) == 0);
    XCTAssertEqual(whitespace->_range.length, 3);

    plang_token_t digit_sequence = plang_next_token(parser);
    XCTAssertNotEqual(digit_sequence, NULL);

    XCTAssertEqual(digit_sequence->_type, plang_token_type_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(digit_sequence), "123", 3) == 0);
    XCTAssertEqual(digit_sequence->_range.length, 3);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(digit_sequence);
    plang_token_free(whitespace);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeDigitSequenceWithLeadingAndTrailingWhitespace
{
    const char *buf = "  \n123\n ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace1 = plang_next_token(parser);
    XCTAssertNotEqual(whitespace1, NULL);

    XCTAssertEqual(whitespace1->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace1), "  \n", 3) == 0);
    XCTAssertEqual(whitespace1->_range.length, 3);

    plang_token_t digit_sequence = plang_next_token(parser);
    XCTAssertNotEqual(digit_sequence, NULL);

    XCTAssertEqual(digit_sequence->_type, plang_token_type_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(digit_sequence), "123", 3) == 0);
    XCTAssertEqual(digit_sequence->_range.length, 3);

    plang_token_t whitespace2 = plang_next_token(parser);
    XCTAssertNotEqual(whitespace2, NULL);

    XCTAssertEqual(whitespace2->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace2), "\n ", 2) == 0);
    XCTAssertEqual(whitespace2->_range.length, 2);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(digit_sequence);
    plang_token_free(whitespace1);
    plang_token_free(whitespace2);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeHexLiteral
{
    const char *buf = "$A9F4";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t token = plang_next_token(parser);
    XCTAssertNotEqual(token, NULL);

    XCTAssertEqual(token->_type, plang_token_type_hex_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(token), "$A9F4", 5) == 0);
    XCTAssertEqual(token->_range.length, 5);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(token);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeHexThenIntegerDigitSequence
{
    const char *buf = "$A9F4 5678";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t hex_digit_sequence = plang_next_token(parser);
    XCTAssertNotEqual(hex_digit_sequence, NULL);

    XCTAssertEqual(hex_digit_sequence->_type, plang_token_type_hex_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(hex_digit_sequence), "$A9F4", 5) == 0);
    XCTAssertEqual(hex_digit_sequence->_range.length, 5);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), " ", 1) == 0);
    XCTAssertEqual(whitespace->_range.length, 1);

    plang_token_t digit_sequence = plang_next_token(parser);
    XCTAssertNotEqual(digit_sequence, NULL);

    XCTAssertEqual(digit_sequence->_type, plang_token_type_digit_sequence);
    XCTAssertTrue(strncmp(plang_token_get_text(digit_sequence), "5678", 4) == 0);
    XCTAssertEqual(digit_sequence->_range.length, 4);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(hex_digit_sequence);
    plang_token_free(whitespace);
    plang_token_free(digit_sequence);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeIdentifier
{
    const char *buf = "Pascal";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t token = plang_next_token(parser);
    XCTAssertNotEqual(token, NULL);

    XCTAssertEqual(token->_type, plang_token_type_identifier);
    XCTAssertTrue(strncmp(plang_token_get_text(token), "Pascal", 6) == 0);
    XCTAssertEqual(token->_range.length, 6);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(token);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeKeyword
{
    const char *buf = "   UNIT  ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    XCTAssertEqual(whitespace->_range.length, 3);

    plang_token_t unit = plang_next_token(parser);
    XCTAssertNotEqual(unit, NULL);

    XCTAssertEqual(unit->_type, plang_token_type_UNIT);
    XCTAssertTrue(strncmp(plang_token_get_text(unit), "UNIT", 4) == 0);
    XCTAssertEqual(unit->_range.length, 4);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(unit);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeStringLiteral
{
    const char *buf = "  'The String''s Literal' ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), "  ", 2) == 0);
    XCTAssertEqual(whitespace->_range.length, 2);

    plang_token_t quoted_string = plang_next_token(parser);
    XCTAssertNotEqual(quoted_string, NULL);

    XCTAssertEqual(quoted_string->_type, plang_token_type_string_literal);
    XCTAssertTrue(strncmp(plang_token_get_text(quoted_string), "'The String''s Literal'", 23) == 0);
    XCTAssertEqual(quoted_string->_range.length, 23);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(quoted_string);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeBraceComment
{
    const char *buf = "   { comment text () }   ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    XCTAssertEqual(whitespace->_range.length, 3);

    plang_token_t comment = plang_next_token(parser);
    XCTAssertNotEqual(comment, NULL);

    XCTAssertEqual(comment->_type, plang_token_type_brace_comment);
    XCTAssertTrue(strncmp(plang_token_get_text(comment), "{ comment text () }", 19) == 0);
    XCTAssertEqual(comment->_range.length, 19);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(comment);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeParenthesesComment
{
    const char *buf = "   (* comment text () *)   ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

    plang_token_t whitespace = plang_next_token(parser);
    XCTAssertNotEqual(whitespace, NULL);

    XCTAssertEqual(whitespace->_type, plang_token_type_whitespace);
    XCTAssertTrue(strncmp(plang_token_get_text(whitespace), "   ", 3) == 0);
    XCTAssertEqual(whitespace->_range.length, 3);

    plang_token_t comment = plang_next_token(parser);
    XCTAssertNotEqual(comment, NULL);

    XCTAssertEqual(comment->_type, plang_token_type_parentheses_comment);
    XCTAssertTrue(strncmp(plang_token_get_text(comment), "(* comment text () *)", 21) == 0);
    XCTAssertEqual(comment->_range.length, 21);

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_token_free(comment);
    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeOneCharacterSpecialSymbols
{
    const char *buf = "  + - * / = < > [ ] . , ( ) : ; ^ @ ";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

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

        XCTAssertNotEqual(token, NULL);

        XCTAssertEqual(token->_type, expected->_type);
        XCTAssertTrue(strncmp(plang_token_get_text(token), expected->_text, 1) == 0);
        XCTAssertEqual(token->_range.length, expected->_text_len);

        plang_token_free(token);
    }

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}

- (void)testTokenizeTwoCharacterSpecialSymbols
{
    const char *buf = "<> <= >= := ..";
    plang_source_t source = plang_source_new(NULL, buf, strlen(buf));
    XCTAssertNotEqual(source, NULL);
    plang_array_t sources = plang_array_new_with_items(source, NULL);
    XCTAssertNotEqual(sources, NULL);
    plang_parser_t parser = plang_parser_new(sources, self.plang_log);
    XCTAssertNotEqual(parser, NULL);

    bool parser_setup = plang_parser_setup(parser);
    XCTAssertTrue(parser_setup);

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

        XCTAssertNotEqual(token, NULL);

        XCTAssertEqual(token->_type, expected->_type);
        XCTAssertTrue(strncmp(plang_token_get_text(token), expected->_text, 2) == 0);
        XCTAssertEqual(token->_range.length, expected->_text_len);

        plang_token_free(token);
    }

    bool parser_teardown = plang_parser_teardown(parser);
    XCTAssertTrue(parser_teardown);

    plang_parser_free(parser);
    plang_source_free(source);
}

@end


NS_ASSUME_NONNULL_END
