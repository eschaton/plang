/*
    plang_token.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_token__h__
#define __plang_token__h__

#include "plang_defines.h"

#include <stdbool.h>

#include "plang_range.h"

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_parser *plang_parser_t;
typedef struct plang_source *plang_source_t;


/*! A `plang` token. */
typedef struct plang_token *plang_token_t;


/*!
 The types of tokens in `plang`.
 */
enum plang_token_type {
    /* General tokens */

    /*!
     Unknown

     An unknown token is used to represent a range of source text when
     reporting an error in tokenization or parsing.
     */
    plang_token_type_unknown = 0,

    /*!
     Whitespace, which is anything from the set `[ \t\r\n]`.
     */
    plang_token_type_whitespace,

    /*!
     Identifier

     An identifier matches `[A-Za-z_][0-9A-Za-z\$_]+` and is not a
     reserved word. Identifiers are case-sensitive but reserved words
     are not.
     */
    plang_token_type_identifier,

    /*!
     Digit Sequence

     A sequence of one or more digits, matches `[0-9]+`.
     */
    plang_token_type_digit_sequence,

    /*!
     Hex Digit Sequence

     A sequence of one or more hex digits, matches `\$[0-9A-Fa-f]+`.
     */
    plang_token_type_hex_digit_sequence,

    /*!
     String Literal

     A string literal is a sequence of any character but CR/LF between
     single-quote `'` characters, with a pair of single-quote characters
     representing a single quote.

     - NOTE: There's no distinct character literal, it's just a
             single-character string.
     */
    plang_token_type_string_literal,

    /*!
     Brace Comment

     A brace comment is all text between `{ }` delimiters, which may
     contain other nested delimiters (but not itself be nested).
     */
    plang_token_type_brace_comment,

    /*!
     Parentheses Comment

     A parentheses comment is all text between `(* *)` delimiters, which
     may contain other nested delimiters (but not itself be nested).
     */
    plang_token_type_parentheses_comment,

    /* Symbols */
    plang_token_type_PLUS,          /*!< '+' */
    plang_token_type_MINUS,         /*!< '-' */
    plang_token_type_ASTERISK,      /*!< '*' */
    plang_token_type_SLASH,         /*!< '/' */
    plang_token_type_EQUALS,        /*!< '=' */
    plang_token_type_LT,            /*!< '<' */
    plang_token_type_GT,            /*!< '>' */
    plang_token_type_LBRACKET,      /*!< ']' */
    plang_token_type_RBRACKET,      /*!< '[' */
    plang_token_type_PERIOD,        /*!< '.' */
    plang_token_type_COMMA,         /*!< ',' */
    plang_token_type_LPAREN,        /*!< '(' */
    plang_token_type_RPAREN,        /*!< ')' */
    plang_token_type_COLON,         /*!< ':' */
    plang_token_type_SEMICOLON,     /*!< ';' */
    plang_token_type_CIRCUMFLEX,    /*!< '^' */
    plang_token_type_AT,            /*!< '@' */
    plang_token_type_LBRACE,        /*!< '{' */
    plang_token_type_RBRACE,        /*!< '}' */

    plang_token_type_NOT_EQUAL,     /*!< '<>' */
    plang_token_type_LTE,           /*!< '<=' */
    plang_token_type_GTE,           /*!< '>=' */
    plang_token_type_ASSIGNMENT,    /*!< ':=' */
    plang_token_type_RANGE,         /*!< '..' */
    plang_token_type_LPCOMMENT,     /*!< '(*' */
    plang_token_type_RPCOMMENT,	    /*!< '*)' */

    /* Reserved words */
    plang_token_type_AND,
    plang_token_type_ARRAY,
    plang_token_type_BEGIN,
    plang_token_type_CASE,
    plang_token_type_CONST,
    plang_token_type_DIV,
    plang_token_type_DOWNTO,
    plang_token_type_DO,
    plang_token_type_ELSE,
    plang_token_type_END,
    plang_token_type_FILE,
    plang_token_type_FOR,
    plang_token_type_FUNCTION,
    plang_token_type_GOTO,
    plang_token_type_IF,
    plang_token_type_IMPLEMENTATION,
    plang_token_type_IN,
    plang_token_type_INTERFACE,
    plang_token_type_LABEL,
    plang_token_type_MOD,
    plang_token_type_NIL,
    plang_token_type_NOT,
    plang_token_type_OF,
    plang_token_type_OR,
    plang_token_type_OTHERWISE,
    plang_token_type_PACKED,
    plang_token_type_PROCEDURE,
    plang_token_type_PROGRAM,
    plang_token_type_RECORD,
    plang_token_type_REPEAT,
    plang_token_type_SET,
    plang_token_type_STRING,
    plang_token_type_THEN,
    plang_token_type_TO,
    plang_token_type_TYPE,
    plang_token_type_UNIT,
    plang_token_type_UNTIL,
    plang_token_type_USES,
    plang_token_type_VAR,
    plang_token_type_WHILE,
    plang_token_type_WITH,
};
typedef enum plang_token_type plang_token_type_t;


/*! Create a token. */
plang_token_t PLANG_NULLABLE
plang_token_new(plang_token_type_t type,
                plang_source_t source,
                const plang_range_t range);


/*!
 Create a token representing an anonymous type identifier.

 An anonymous type identifier is an identifier synthesized for the type
 declared in the given source and range, which is distinct from but
 congruent to all other types with an equivalent declaration.
 */
plang_token_t PLANG_NULLABLE
plang_token_new_anonymous_type_identifier(plang_source_t source,
                                          const plang_range_t range);


/*! Dispose of a token. */
void
plang_token_free(plang_token_t PLANG_NULLABLE token);


/*! Get the type of the token. */
plang_token_type_t
plang_token_get_type(plang_token_t token);


/*! Get the source of the token. */
plang_source_t
plang_token_get_source(plang_token_t token);


/*!
 Get the range of the input that a token spans.
 */
plang_range_t
plang_token_get_range(plang_token_t token);


/*!
 Get the text of the token.

 - WARNING: The text is **NOT** `NUL`-terminated, it is just a pointer
            into the token's source, so it must always be used with its
            length.
 */
const char * PLANG_NULLABLE
plang_token_get_text(plang_token_t token);


/*!
 Get a copy of the text of the token.

 - WARNING: The memory for the text is allocated with ``calloc(3)`` and
            must be freed with ``free(3)`` by the caller.
 */
const char * PLANG_NULLABLE
plang_token_copy_text(plang_token_t PLANG_NULLABLE token);


/*! Compare identifier token with a string for equality. */
bool
plang_token_identifier_equals(plang_token_t PLANG_NULLABLE token,
                              const char *string);


/*!
 Compare identifier tokens for textual equality.

 This is to support the use of identifier tokens as dictionary keys.
 */
bool
plang_token_identifier_comparator(void *key1,
                                  void *key2,
                                  void * PLANG_NULLABLE context);



/*! Check whether a token exists or is of the expected type. */
bool
plang_token_matches(plang_token_t PLANG_NULLABLE token,
                    plang_token_type_t type);


/*!
 Does the token represent a built-in type?

 A built-in type token is an identifier token that has the prefix
 `__BUILT_IN_` and one of the following suffixes:

 - `INT8`, an 8-bit integer
 - `INT16`, a 16-bit integer
 - `INT32`, a 32-bit integer
 - `INT64`, a 64-bit integer
 - `INTPTR`, an integer of correct size to hold a pointer
 - `BOOLEAN`, a logical `FALSE` or `TRUE`
 - `CHAR`, a character in a string
 - `ANYPTR`, the polymorphic type

 These are used to define the actual built-in types in the preamble, and
 are the lowest level of the type system upon which the rest is built.
 */
bool
plang_token_identifier_is_built_in_type(plang_token_t token);


/*!
 Is the token a built-in ordinal type?

 Tells whether the built-in type is one of the built-in ordinal types.
 (Currently all built-in types except `__BUILT_IN_ANYPTR`, which is
 the polymorphic type taken by the built-in memory allocator.)
 */
bool
plang_token_identifier_is_built_in_ordinal_type(plang_token_t token);


/*!
 Is the token a built-in pointer type?

 Tells whether the built-in type is one of the built-in pointer types.
 (Currently only `__BUILT_IN_ANYPTR`, which is the polymorphic type
 taken by the built-in memory allocator.)
 */
bool
plang_token_identifier_is_built_in_pointer_type(plang_token_t token);



/*!
 Is the token a _term_ operator?

 Indicates whether the token represents one of the operators in the
 _term_ rule of the language grammar.
 */
bool
plang_token_is_factor_operator(plang_token_t PLANG_NULLABLE token);


/*!
 Is the token a _simple-expression_ operator?

 Indicates whether the token represents one of the operators in the
 _simple-expression_ rule of the language grammar.
 */
bool
plang_token_is_simple_expression_operator(plang_token_t PLANG_NULLABLE token);


/*!
 Is the token an _expression_ operator?

 Indicates whether the token represents one of the operators in the
 _expression_ rule of the language grammar.
 */
bool
plang_token_is_expression_operator(plang_token_t PLANG_NULLABLE token);


/*!
 Gets the next token
 
 Runs the tokenizer to get the next token, and updates the tokenizer
 state within the parser. Returns `NULL` if there are no more tokens.
 */
plang_token_t PLANG_NULLABLE
plang_next_token(plang_parser_t parser);


PLANG_HEADER_END

#endif /* __plang_token__h__ */
