/*
    plang.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang__h__
#define __plang__h__

#include "plang_defines.h"

/* Tokenizing & Parsing */

#include "plang_error.h"
#include "plang_node.h"
#include "plang_parser.h"
#include "plang_range.h"
#include "plang_source.h"
#include "plang_token.h"

/* Higher-Level Constructs */

#include "plang_constant.h"
#include "plang_function.h"
#include "plang_procedure.h"
#include "plang_program.h"
#include "plang_scope.h"
#include "plang_type.h"
#include "plang_unit.h"
#include "plang_variable.h"

/* Utilities */

#include "plang_array.h"
#include "plang_dictionary.h"
#include "plang_log.h"


/* Driver */

/*!
 A minimal but complete driver for `plang`.

 This will create and run the parser on a set of sources, optionally
 with a preamble file, collect any errors that occur, and output them.

 - Parameters:
   - preamble: A preamble file to use, if any.
   - sources: Source files, in compilation order.
   - log: The log to write to.

 - Returns: The parser that was used by the driver, or `NULL` if an
            error occurred before a parser could be set up.
 */
plang_parser_t PLANG_NULLABLE
plang_driver(plang_source_t PLANG_NULLABLE preamble,
             plang_array_t PLANG_NONNULL sources,
             plang_log_t PLANG_NONNULL log);


#endif /* __plang_h__ */
