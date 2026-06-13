/*
    plang_scope.h
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#ifndef __plang_scope__h__
#define __plang_scope__h__

#include "plang_defines.h"

#include <stdbool.h>

PLANG_HEADER_BEGIN


/* Forward declarations. */

typedef struct plang_constant *plang_constant_t;
typedef struct plang_dictionary *plang_dictionary_t;
typedef struct plang_function *plang_function_t;
typedef struct plang_procedure *plang_procedure_t;
typedef struct plang_token *plang_token_t;
typedef struct plang_type *plang_type_t;
typedef struct plang_variable *plang_variable_t;


/*! A nestable lexical scope. */
typedef struct plang_scope *plang_scope_t;


/*!
 Creates a scope.

 Creates a new scope associated with \a parser and nested within the
 \a parent scope, if any.
 */
plang_scope_t PLANG_NULLABLE
plang_scope_new(plang_scope_t PLANG_NULLABLE parent);


/*!
 Dispose of a scope.

 Does not dissociate the scope from any children.
 */
void
plang_scope_free(plang_scope_t PLANG_NULLABLE scope);


/*! Gets the parent scope of a scope, if any. */
plang_scope_t PLANG_NULLABLE
plang_scope_get_parent(plang_scope_t scope);


/*!
 Register a constant with the scope.

 Registration will fail if the constant is already represented within
 the scope. However, shadowing is allowed.
 */
bool
plang_scope_constant_register(plang_scope_t scope,
                              plang_constant_t constant);


/*!
 Look up a constant.

 Searches for a constant indicated by \a identifier within \a scope,
 optionally searching parent scopes as well.
 */
plang_constant_t PLANG_NULLABLE
plang_scope_constant_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents);


/*!
 Get a copy of all the constants in the scope.
 */
plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_constants(plang_scope_t scope);


/*!
 Register a type with the scope.

 Registration will fail if the type is already represented within
 the scope. However, shadowing is allowed.
 */
bool
plang_scope_type_register(plang_scope_t scope,
                          plang_type_t type);


/*!
 Look up a type.

 Searches for a type indicated by \a identifier within \a scope,
 optionally searching parent scopes as well.
 */
plang_type_t PLANG_NULLABLE
plang_scope_type_lookup(plang_scope_t scope,
                        plang_token_t identifier,
                        bool search_parents);


/*!
 Get a copy of all the types in the scope.
 */
plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_types(plang_scope_t scope);


/*!
 Register a variable with the scope.

 Registration will fail if the variable is already represented within
 the scope. However, shadowing is allowed.
 */
bool
plang_scope_variable_register(plang_scope_t scope,
                              plang_variable_t variable);


/*!
 Look up a variable.

 Searches for a variable indicated by \a identifier within \a scope,
 optionally searching parent scopes as well.
 */
plang_variable_t PLANG_NULLABLE
plang_scope_variable_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents);


/*!
 Get a copy of all the variables in the scope.
 */
plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_variables(plang_scope_t scope);


/*!
 Register a procedure with the scope.

 Registration will fail if the procedure is already represented within
 the scope. However, shadowing is allowed.
 */
bool
plang_scope_procedure_register(plang_scope_t scope,
                               plang_procedure_t procedure);


/*!
 Look up a procedure.

 Searches for a procedure indicated by \a identifier within \a scope,
 optionally searching parent scopes as well.
 */
plang_procedure_t PLANG_NULLABLE
plang_scope_procedure_lookup(plang_scope_t scope,
                             plang_token_t identifier,
                             bool search_parents);


/*!
 Get a copy of all the procedures in the scope.
 */
plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_procedures(plang_scope_t scope);


/*!
 Register a function with the scope.

 Registration will fail if the function is already represented within
 the scope. However, shadowing is allowed.
 */
bool
plang_scope_function_register(plang_scope_t scope,
                              plang_function_t function);


/*!
 Look up a function.

 Searches for a function indicated by \a identifier within \a scope,
 optionally searching parent scopes as well.
 */
plang_function_t PLANG_NULLABLE
plang_scope_function_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents);


/*!
 Get a copy of all the functions in the scope.
 */
plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_functions(plang_scope_t scope);


PLANG_HEADER_END

#endif /* __plang_scope__h__ */
