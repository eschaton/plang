/*
    plang_parser.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_parser__h__
#define __plang_parser__h__

#include "plang_defines.h"

#include <stdbool.h>
#include <unistd.h>

PLANG_HEADER_BEGIN


/* Forward Declarations */

typedef struct plang_array *plang_array_t;
typedef struct plang_dictionary *plang_dictionary_t;
typedef struct plang_error *plang_error_t;
typedef struct plang_node *plang_node_t;
typedef struct plang_program *plang_program_t;
typedef struct plang_scope *plang_scope_t;
typedef struct plang_source *plang_source_t;
typedef struct plang_token *plang_token_t;
typedef struct plang_unit *plang_unit_t;


/*!
 A `plang` parser.
 */
typedef struct plang_parser *plang_parser_t;


/*!
 Constructor for the `plang` parser.

 Create a parser ready to parse the source text in the array of sources,
 which are each instances of ``plang_source_t``.
 */
plang_parser_t PLANG_NULLABLE
plang_parser_new(plang_array_t sources);


/*!
 Destructor for the `plang` parser.

 Dispose of a `plang` parser and associated data structures.
 */
void
plang_parser_free(plang_parser_t PLANG_NULLABLE parser);


/*!
 Set up the parser before a run.

 Ensures the next source will be parsed and that there's a scope for it.

 - WARNING: Only necessary if using the tokenizer directly.
 */
bool
plang_parser_setup(plang_parser_t parser);


/*!
 Set a preamble for the parser to use.

 Tells the parser that a specific source should be treated as a preamble
 whose interface scope is the topmost scope while parsing other sources.

 Setting the preamble to `NULL` will cause no preamble to be used.
 */
void
plang_parser_set_preamble(plang_parser_t parser,
                          plang_source_t PLANG_NULLABLE source);


/*!
 Get the preamble the parser is using, if any.
 */
plang_source_t PLANG_NULLABLE
plang_parser_get_preamble(plang_parser_t parser);


/*!
 Get a copy of the array of sources the parser is using.
 */
plang_array_t
plang_parser_copy_sources(plang_parser_t parser);


/*!
 Run the parser.

 Parses the source text, returning `true` if parsing succeeded.

 - NOTE: "Success" is in the eye of the beholder: A parse can be
         successful while still containing errors whether syntactic or
         semantic.
 */
bool
plang_parser_run(plang_parser_t parser);


/*!
 Tear down the parser after a run.

 Does any cleanup necessary after running a parse.

 - WARNING: Only necessary if using the tokenizer directly.
 */
bool
plang_parser_teardown(plang_parser_t parser);


/*! Get the next token, even if it's whitespace or a comment. */
plang_token_t PLANG_NULLABLE
plang_parser_next_absolute_token(plang_parser_t parser);


/*! Get the next significant (non-whitespace, non-comment) token. */
plang_token_t PLANG_NULLABLE
plang_parser_next_significant_token(plang_parser_t parser);


/*!
 "Return" a token to the collection of next tokens.

 This is the parser equivalent to the tokenizer's use of "un-getting" a
 character while tokenizing text; it allows the parser to try and then
 back out of a parsing rule.
 */
void
plang_parser_return_token(plang_parser_t parser,
                          plang_token_t token);


/*! Get the current parser source. */
plang_source_t
plang_parser_get_source(plang_parser_t parser);


/*! Get the current parser position. */
size_t
plang_parser_get_position(plang_parser_t parser);


/* MARK: - Scope Management */

/*!
 Push a new scope.

 Set the current parsing scope to a new scope.
 */
bool
plang_parser_scope_push(plang_parser_t parser,
                        plang_scope_t scope);


/*!
 Pop to the current scope's parent.

 Set the current parsing scope to the current scope's parent, and
 returns that scope.

 - WARNING: Will assert if the current scope has no parent.
 */
plang_scope_t PLANG_NULLABLE
plang_parser_scope_pop(plang_parser_t parser);


/*!
 Get the current scope.

 Returns the current parsing scope.
 */
plang_scope_t
plang_parser_scope_current(plang_parser_t parser);


/* MARK: - Error Handling */

/*!
 Register an error with the parser.
 */
void
plang_parser_signal_error(plang_parser_t parser,
                          plang_error_t error);


/*!
 Get a copy of all parse errors.

 Returns an array containing references to all of the errors encountered
 during parsing, or `NULL` if there aren't any. Ownership of the array
 is passed to the caller, but the referenced errors are still owned by
 the parser.
 */
plang_array_t PLANG_NULLABLE
plang_parser_copy_errors(plang_parser_t parser);


/* MARK: - Result Handling */

/*!
 Get the parsed program.

 If there hasn't been a program parsed yet, returns `NULL`.
 */
plang_program_t PLANG_NULLABLE
plang_parser_get_program(plang_parser_t parser);


/*!
 Get the parsed preamble unit.

 If there hasn't been a preamble unit parsed yet, returns `NULL`.

 - NOTE: Any preamble unit is **not** included in the array returned by
         ``plang_parser_copy_all_units``.
 */
plang_unit_t PLANG_NULLABLE
plang_parser_get_preamble_unit(plang_parser_t parser);


/*!
 Get the parsed unit by its identifier.

 Returns `NULL` if there isn't one for the given identifier.
 */
plang_unit_t PLANG_NULLABLE
plang_parser_get_unit(plang_parser_t parser,
                      plang_token_t identifier);


/*!
 Get a copy of the dictionary referencing all parsed units.

 The dictionary returned is pre-configured to have its keys compared
 via ``plang_token_identifier_comparator``.

 - WARNING: The dictionary is the caller's responsibility to release,
            but the dictionary's contents are all owned by the parser.
 */
plang_dictionary_t PLANG_NULLABLE
plang_parser_copy_all_units(plang_parser_t parser);


PLANG_HEADER_END

#endif /* __plang_parser__h__ */
