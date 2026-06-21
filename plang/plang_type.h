/*
    plang_type.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_type__h__
#define __plang_type__h__

#include "plang_defines.h"

#include <stdbool.h>

PLANG_HEADER_BEGIN


/* Forward Declarations*/

typedef struct plang_node *plang_node_t;
typedef struct plang_parser *plang_parser_t;
typedef struct plang_scope *plang_scope_t;
typedef struct plang_token *plang_token_t;


/*! A type in `plang`. */
typedef struct plang_type *plang_type_t;


plang_type_t PLANG_NULLABLE
plang_type_new(plang_parser_t parser,
               plang_token_t identifier,
               plang_node_t node);


void
plang_type_free(plang_type_t PLANG_NULLABLE type);


plang_parser_t
plang_type_get_parser(plang_type_t type);


plang_token_t
plang_type_get_identifier(plang_type_t type);


plang_node_t
plang_type_get_node(plang_type_t type);


/*!
 The kinds of type supported by `plang`.

 There are several different kinds of types supported by `plang`:
 Ordinal types, real types, string types, structured types, and
 pointer types.

 - NOTE: This is slightly different than the predicate functions below,
         since records are one kind of structured type.
 */
typedef enum {
    plang_type_kind_unknonwn = 0,
    plang_type_kind_ordinal,
    plang_type_kind_real,
    plang_type_kind_string,
    plang_type_kind_structured,
    plang_type_kind_pointer,
} plang_type_kind_t;


/*! Get the kind of type that a type represents. */
plang_type_kind_t
plang_type_get_kind(plang_type_t type);


/*!
 Indicates whether a type is concrete.

 A _concrete type_ is a type where right-hand side of its declaration is
 a type or one of the built-in type identifiers. An _alias type_ is one
 where the right-hand side of its declaration is a type identifier that
 must be resolved for details about the type.
 */
bool
plang_type_is_concrete(plang_type_t type);


/*!
 Resolve the type's most concrete node and kind.

 Before a type is resolved, whether it's a concrete type or alias type
 isn't known. This resolution must be performed after the type is fully
 created but before it is used.

 - Returns: `true` if resolution succeeds, `false` if it fails for some
            reason
 */
bool
plang_type_resolve(plang_type_t type,
                   plang_scope_t scope);


/*!
 Get the most concrete type for the given type.

 A ``plang_type_t`` may directly represent a type, or it may represent
 an alias of the type. As an example, in this _type-declaration-part_,

     TYPE
         INT8 = -128..127;
         BYTE = INT8;

 the type `BYTE` is an alias of the type `INT8`, which in turn is an
 alias for a range type. So this function, when given `BYTE`, will
 return the ``plang_type_t`` for that range type.

 - NOTE: This function takes a scope for resolving identifiers.
 */
plang_node_t PLANG_NULLABLE
plang_type_get_concrete_type_node(plang_type_t PLANG_NULLABLE type,
                                  plang_scope_t scope);


bool
plang_type_is_ordinal(plang_type_t PLANG_NULLABLE type,
                      plang_scope_t scope);


bool
plang_type_is_real(plang_type_t PLANG_NULLABLE type,
                   plang_scope_t scope);


bool
plang_type_is_string(plang_type_t PLANG_NULLABLE type,
                     plang_scope_t scope);


bool
plang_type_is_structured(plang_type_t PLANG_NULLABLE type,
                         plang_scope_t scope);


bool
plang_type_is_record(plang_type_t PLANG_NULLABLE type,
                     plang_scope_t scope);


bool
plang_type_is_pointer(plang_type_t PLANG_NULLABLE type,
                      plang_scope_t scope);


PLANG_HEADER_END

#endif /* __plang_type__h__ */
