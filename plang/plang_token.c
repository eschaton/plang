/*
    plang_token.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_token_internal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "plang_error.h"
#include "plang_source.h"

#include "plang_parser_internal.h"

PLANG_SOURCE_BEGIN


plang_token_t PLANG_NULLABLE
plang_token_new(plang_token_type_t type,
                plang_source_t source,
                const plang_range_t range)
{
    plang_token_t token = calloc(sizeof(struct plang_token), 1);
    if (token) {
        token->_type = type;
        token->_source = source;
        token->_range = range;
    }

    return token;
}


void
plang_token_free(plang_token_t PLANG_NULLABLE token)
{
    if (token) {
        free(token);
    }
}


plang_token_type_t
plang_token_get_type(plang_token_t token)
{
    return token->_type;
}


plang_source_t
plang_token_get_source(plang_token_t token)
{
    return token->_source;
}


plang_range_t
plang_token_get_range(plang_token_t token)
{
    return token->_range;
}


const char * PLANG_NULLABLE
plang_token_get_text(plang_token_t token)
{
    if (token->_range.length == 0) return NULL;
    return plang_source_get_chars(token->_source, token->_range.start);
}


const char * PLANG_NULLABLE
plang_token_copy_text(plang_token_t PLANG_NULLABLE token)
{
    char *text = NULL;
    
    text = calloc(sizeof(char), token->_range.length + 1);
    if (text) {
        strlcpy(text,
                plang_source_get_chars(token->_source,
                                       token->_range.start),
                token->_range.length + 1);
    }
    
    return text;
}


bool
plang_token_identifier_equals(plang_token_t PLANG_NULLABLE token,
                              const char *string)
{
    if (token == NULL) return false;

    const size_t string_len = strlen(string);

    /* Do trivial rejections first. */

    if (token->_range.length != string_len) return false;

    /* Do expensive comparison last. */

    const char *text = plang_token_get_text(token);

    return (strncasecmp(text, string, string_len) == 0);
}


bool plang_token_identifier_comparator(void *key1,
                                       void *key2,
                                       void * PLANG_NULLABLE context)
{
    /* Do trivial rejections first. */

    plang_token_t id1 = key1;
    plang_token_t id2 = key2;

    if (id1 == id2) return true;

    plang_range_t r1 = plang_token_get_range(id1);
    plang_range_t r2 = plang_token_get_range(id2);

    if (r1.length != r2.length) return false;

    /* Do expensive comparison last. */

    const char *t1 = plang_token_get_text(id1);
    const char *t2 = plang_token_get_text(id2);

    if (strncmp(t1, t2, r1.length) == 0) return true;

    return false;
}


bool
plang_token_matches(plang_token_t PLANG_NULLABLE token,
                    plang_token_type_t type)
{
    return ((token != NULL) && (plang_token_get_type(token) == type));
}


bool
plang_token_is_factor_operator(plang_token_t PLANG_NULLABLE token)
{
    if (token == NULL) return false;
    
    switch (token->_type) {
        case plang_token_type_ASTERISK:
        case plang_token_type_SLASH:
        case plang_token_type_DIV:
        case plang_token_type_MOD:
        case plang_token_type_AND:
            return true;
            
        default:
            return false;
    }
}


bool
plang_token_is_simple_expression_operator(plang_token_t PLANG_NULLABLE token)
{
    if (token == NULL) return false;

    switch (token->_type) {
        case plang_token_type_PLUS:
        case plang_token_type_MINUS:
        case plang_token_type_OR:
            return true;
            
        default:
            return false;
    }
}


bool
plang_token_is_expression_operator(plang_token_t PLANG_NULLABLE token)
{
    if (token == NULL) return false;

    switch (token->_type) {
        case plang_token_type_EQUALS:
        case plang_token_type_LT:
        case plang_token_type_GT:
        case plang_token_type_LTE:
        case plang_token_type_GTE:
        case plang_token_type_NOT_EQUAL:
        case plang_token_type_IN:
            return true;

        default:
            return false;
    }
}


static
bool
plang_char_is_whitespace(char ch)
{
    return ((ch == ' ') ||
            (ch == '\t') ||
            (ch == '\n') ||
            (ch == '\r'));
}


static
bool
plang_char_is_identifier_start(char ch)
{
    return (((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= 'a') && (ch <= 'z')) ||
            (ch == '_') || (ch == '$'));
}


static
bool
plang_char_is_identifier(char ch)
{
    return (((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= 'a') && (ch <= 'z')) ||
            ((ch >= '0') && (ch <= '9')) ||
            (ch == '_') || (ch == '$'));
}


static
bool
plang_char_is_decimal_digit(char ch)
{
    return ((ch >= '0') && (ch <= '9'));
}


static
bool
plang_char_is_hex_digit(char ch)
{
    return (((ch >= '0') && (ch <= '9')) ||
            ((ch >= 'A') && (ch <= 'F')) ||
            ((ch >= 'a') && (ch <= 'f')));
}


static
bool
plang_char_is_special_symbol(char ch)
{
    switch (ch) {
        case '+': case '-': case '*': case '/': case '=':
        case '<': case '>': case '[': case ']': case '.':
        case ',': case '(': case ')': case ':': case ';':
        case '^': case '@': case '{': case '}':
            return true;

        default:
            return false;
    }
}


plang_token_t PLANG_NULLABLE
plang_tokenize_whitespace(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(plang_char_is_whitespace(ch));

    do {
        ch = plang_get_char(parser);
    } while (plang_char_is_whitespace(ch));
    plang_unget_char(parser, ch);

    token_range.length = parser->_source_pos - token_range.start;

    token = plang_token_new(plang_token_type_whitespace,
                            parser->_source,
                            token_range);

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_hex_digit_sequence(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(ch == '$');

    do {
        ch = plang_get_char(parser);
    } while (plang_char_is_hex_digit(ch));
    plang_unget_char(parser, ch);

    token_range.length = parser->_source_pos - token_range.start;

    if (token_range.length > 1) {
        token = plang_token_new(plang_token_type_hex_digit_sequence,
                                parser->_source,
                                token_range);
    } else {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_character,
                              parser->_source,
                              token_range);
        assert(error != NULL);
        plang_parser_signal_error(parser, error);
    }

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_digit_sequence(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(plang_char_is_decimal_digit(ch));

    do {
        ch = plang_get_char(parser);
    } while (plang_char_is_decimal_digit(ch));
    plang_unget_char(parser, ch);

    token_range.length = parser->_source_pos - token_range.start;

    token = plang_token_new(plang_token_type_digit_sequence,
                            parser->_source,
                            token_range);

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_string_literal(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(ch == '\'');

    bool done = false;
    bool saw_unexpected_end_of_text = false;
    bool saw_invalid_character = false;
    while (!done) {
        ch = plang_get_char(parser);
        if (ch == '\0') {
            /* Unexpected end of text */
            done = true;
        } else if ((ch == '\n') || (ch == '\r')) {
            /* Invalid character */
            done = true;
        } else if (ch == '\'') {
            /* Quoted ' or end of string */
            char next_ch = plang_peek_char(parser);
            if (next_ch == '\'') {
                /* Quoted quote, skip it. */
                (void) plang_get_char(parser);
            } else {
                done = true;
            }
        } else {
            /* Keep scanning. */
        }
    }

    token_range.length = parser->_source_pos - token_range.start;

    if (saw_unexpected_end_of_text || saw_invalid_character) {
        plang_error_t error = NULL;
        if (saw_unexpected_end_of_text) {
            error = plang_error_new(plang_error_type_unexpected_end_of_text,
                                    parser->_source,
                                    token_range);
        } else if (saw_invalid_character) {
            error = plang_error_new(plang_error_type_unexpected_character,
                                    parser->_source,
                                    token_range);
        }
        assert(error != NULL);
        plang_parser_signal_error(parser, error);

    } else {
        token = plang_token_new(plang_token_type_string_literal,
                                parser->_source,
                                token_range);
    }

    return token;
}


void
plang_tokenize_reserved_word(plang_parser_t parser,
                             plang_token_t token)
{
    struct plang_reserved_word {
        const char *_text;
        const size_t _text_len;
        const plang_token_type_t _type;
    } reserved_words[] = {
        { "AND", 3, plang_token_type_AND },
        { "ARRAY", 5, plang_token_type_ARRAY },
        { "BEGIN", 5, plang_token_type_BEGIN },
        { "CASE", 4, plang_token_type_CASE },
        { "CONST", 5, plang_token_type_CONST },
        { "DIV", 3, plang_token_type_DIV },
        { "DOWNTO", 6, plang_token_type_DOWNTO },
        { "DO", 2, plang_token_type_DO },
        { "ELSE", 4, plang_token_type_ELSE },
        { "END", 3, plang_token_type_END },
        { "FILE", 4, plang_token_type_FILE },
        { "FOR", 3, plang_token_type_FOR },
        { "FUNCTION", 8, plang_token_type_FUNCTION },
        { "GOTO", 4, plang_token_type_GOTO },
        { "IF", 2, plang_token_type_IF },
        { "IMPLEMENTATION", 14, plang_token_type_IMPLEMENTATION },
        { "IN", 2, plang_token_type_IN },
        { "INTERFACE", 9, plang_token_type_INTERFACE },
        { "LABEL", 5, plang_token_type_LABEL },
        { "MOD", 3, plang_token_type_MOD },
        { "NIL", 3, plang_token_type_NIL },
        { "NOT", 3, plang_token_type_NOT },
        { "OF", 2, plang_token_type_OF },
        { "OR", 2, plang_token_type_OR },
        { "OTHERWISE", 9, plang_token_type_OTHERWISE },
        { "PACKED", 6, plang_token_type_PACKED },
        { "PROCEDURE", 9, plang_token_type_PROCEDURE },
        { "PROGRAM", 7, plang_token_type_PROGRAM },
        { "RECORD", 6, plang_token_type_RECORD },
        { "REPEAT", 6, plang_token_type_REPEAT },
        { "SET", 3, plang_token_type_SET },
        { "STRING", 6, plang_token_type_STRING },
        { "THEN", 4, plang_token_type_THEN },
        { "TO", 2, plang_token_type_TO },
        { "TYPE", 4, plang_token_type_TYPE },
        { "UNIT", 4, plang_token_type_UNIT },
        { "UNTIL", 5, plang_token_type_UNTIL },
        { "USES", 4, plang_token_type_USES },
        { "VAR", 3, plang_token_type_VAR },
        { "WHILE", 5, plang_token_type_WHILE },
        { "WITH", 4, plang_token_type_WITH },
        { NULL, 0, plang_token_type_unknown },
    };

    for (struct plang_reserved_word *word = &reserved_words[0];
         word->_text != NULL;
         word = word + 1)
    {
        const char *text = plang_token_get_text(token);
        const plang_range_t range = plang_token_get_range(token);

        /* Do a trivial rejection first, before full string compare. */

        if ((word->_text_len == range.length) &&
            (strncasecmp(text, word->_text, word->_text_len) == 0))
        {
            token->_type = word->_type;
            break;
        }
    }
}


plang_token_t PLANG_NULLABLE
plang_tokenize_identifier(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(plang_char_is_identifier_start(ch));

    do {
        ch = plang_get_char(parser);
    } while (plang_char_is_identifier(ch));
    plang_unget_char(parser, ch);

    token_range.length = parser->_source_pos - token_range.start;

    token = plang_token_new(plang_token_type_identifier,
                            parser->_source,
                            token_range);

    /* Recognize identifiers representing reserved words */

    plang_tokenize_reserved_word(parser, token);

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_parentheses_comment(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch0 = plang_get_char(parser);
    char ch1 = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert((ch0 == '(') && (ch1 == '*'));

    // TODO: Implement comment nesting.

    bool done = false;
    bool succeeded = false;
    while (!done) {
        char ch3 = plang_get_char(parser);
        if (ch3 == '*') {
            char ch4 = plang_peek_char(parser);
            if (ch4 == ')') {
                (void) plang_get_char(parser);
                done = true;
                succeeded = true;
            }
        } else if (ch3 == '\0') {
            /* End of text */
            done = true;
        }
    }

    token_range.length = parser->_source_pos - token_range.start;

    if (!succeeded) {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_end_of_text,
                              parser->_source,
                              token_range);
        assert(error != NULL);
        plang_parser_signal_error(parser, error);
    } else {
        token = plang_token_new(plang_token_type_parentheses_comment,
                                parser->_source,
                                token_range);
    }

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_brace_comment(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(ch == '{');

    // TODO: Implement comment nesting.

    do {
        ch = plang_get_char(parser);
    } while ((ch != '}') && (ch != '\0'));

    token_range.length = parser->_source_pos - token_range.start;

    if (ch == '\0') {
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_end_of_text,
                              parser->_source,
                              token_range);
        assert(error != NULL);
        plang_parser_signal_error(parser, error);
    } else {
        token = plang_token_new(plang_token_type_brace_comment,
                                parser->_source,
                                token_range);
    }

    return token;
}


plang_token_t PLANG_NULLABLE
plang_tokenize_special_symbol(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_get_char(parser);

    /* Should not get here without the following being true */
    assert(plang_char_is_special_symbol(ch));

    token_range.length = 1;
    plang_token_type_t type;

    switch (ch) {
        case '+': { type = plang_token_type_PLUS; } break;
        case '-': { type = plang_token_type_MINUS; } break;

        case '*': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == ')') {
                (void) plang_get_char(parser);
                type = plang_token_type_RPCOMMENT;
                token_range.length = 2;
            } else {
                type = plang_token_type_ASTERISK;
            }
        } break;

        case '/': { type = plang_token_type_SLASH; } break;
        case '=': { type = plang_token_type_EQUALS; } break;

        case '<': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == '=') {
                (void) plang_get_char(parser);
                type = plang_token_type_LTE;
                token_range.length = 2;
            } else if (next_ch == '>') {
                (void) plang_get_char(parser);
                type = plang_token_type_NOT_EQUAL;
                token_range.length = 2;
            } else {
                type = plang_token_type_LT;
            }
        } break;

        case '>': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == '=') {
                (void) plang_get_char(parser);
                type = plang_token_type_GTE;
                token_range.length = 2;
            } else {
                type = plang_token_type_GT;
            }
        } break;

        case '[': { type = plang_token_type_LBRACKET; } break;
        case ']': { type = plang_token_type_RBRACKET; } break;

        case '.': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == '.') {
                (void) plang_get_char(parser);
                type = plang_token_type_RANGE;
                token_range.length = 2;
            } else {
                type = plang_token_type_PERIOD;
            }
        } break;

        case ',': { type = plang_token_type_COMMA; } break;

        case '(': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == '*') {
                (void) plang_get_char(parser);
                type = plang_token_type_LPCOMMENT;
                token_range.length = 2;
            } else {
                type = plang_token_type_LPAREN;
            }
        } break;

        case ')': { type = plang_token_type_RPAREN; } break;

        case ':': {
            char next_ch = plang_peek_char(parser);
            if (next_ch == '=') {
                (void) plang_get_char(parser);
                type = plang_token_type_ASSIGNMENT;
                token_range.length = 2;
            } else {
                type = plang_token_type_COLON;
            }
        } break;

        case ';': { type = plang_token_type_SEMICOLON; } break;
        case '^': { type = plang_token_type_CIRCUMFLEX; } break;
        case '@': { type = plang_token_type_AT; } break;

        case '{': { type = plang_token_type_LBRACE; } break;
        case '}': { type = plang_token_type_RBRACE; } break;

        default: {
            type = plang_token_type_unknown;
        } break;
    }

    /* Also handle comments introduced by these tokens. */
    if (type == plang_token_type_LPCOMMENT) {
        plang_unget_char(parser, '*');
        plang_unget_char(parser, '(');
        token = plang_tokenize_parentheses_comment(parser);
    } else if (type == plang_token_type_LBRACE) {
        plang_unget_char(parser, ch);
        token = plang_tokenize_brace_comment(parser);
    } else {
        token = plang_token_new(type, parser->_source, token_range);
    }

    return token;
}


plang_token_t PLANG_NULLABLE
plang_next_token(plang_parser_t parser)
{
    plang_token_t token = NULL;

    plang_range_t token_range;
    token_range.start = parser->_source_pos;

    char ch = plang_peek_char(parser);
    if (ch == '\0') {
        /* At end of text. */
        return NULL;
    } else if (plang_char_is_whitespace(ch)) {
        token = plang_tokenize_whitespace(parser);
    } else if (ch == '$') {
        /* Introduces hex literal */
        token = plang_tokenize_hex_digit_sequence(parser);
    } else if (ch == '-') {
        /* Introduces signed integer literal or is MINUS. */
        (void) plang_get_char(parser);
        char next_char = plang_peek_char(parser);
        if (plang_char_is_decimal_digit(next_char)) {
            token = plang_tokenize_digit_sequence(parser);
            if (token) {
                token->_type = plang_token_type_digit_sequence;
                token->_range.start -= 1;
                token->_range.length += 1;
            } else {
                token_range.length = 1;
                token = plang_token_new(plang_token_type_MINUS,
                                        parser->_source,
                                        token_range);
            }
        } else {
            token_range.length = 1;
            token = plang_token_new(plang_token_type_MINUS,
                                    parser->_source,
                                    token_range);
        }
    } else if (ch == '\'') {
        /* Introduces string literal */
        token = plang_tokenize_string_literal(parser);
    } else if (plang_char_is_decimal_digit(ch)) {
        /* Introduces a numeric literal. */
        token = plang_tokenize_digit_sequence(parser);
    } else if (plang_char_is_identifier_start(ch)) {
        /* Introduces identifier or reserved word */
        token = plang_tokenize_identifier(parser);
    } else if (plang_char_is_special_symbol(ch)) {
        /* Introducse 1-2 character symbolic token, also comments */
        token = plang_tokenize_special_symbol(parser);
    } else {
        /* Unknown */
        token_range.length = parser->_source_pos - token_range.start;
        plang_error_t error
            = plang_error_new(plang_error_type_unexpected_character,
                              parser->_source,
                              token_range);
        assert(error != NULL);
        plang_parser_signal_error(parser, error);
    }

    return token;
}


PLANG_SOURCE_END
