/*
    plang_scope.c
    plang

    Copyright © 2026 Christopher M. Hanson. All rights reserved.
    See the file COPYING for license details.
*/

#include "plang_scope_internal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "plang_array.h"
#include "plang_constant.h"
#include "plang_dictionary.h"
#include "plang_function.h"
#include "plang_procedure.h"
#include "plang_token.h"
#include "plang_type.h"
#include "plang_variable.h"

PLANG_SOURCE_BEGIN


plang_scope_t PLANG_NULLABLE
plang_scope_new(plang_scope_t PLANG_NULLABLE parent)
{
    struct plang_scope *scope = calloc(sizeof(struct plang_scope), 1);
    if (scope == NULL) goto error;

    scope->_parent = parent;

    scope->_constants = plang_dictionary_new(4);
    if (scope->_constants == NULL) goto error;
    plang_dictionary_set_key_comparator(scope->_constants,
                                        plang_token_identifier_comparator,
                                        NULL);

    scope->_types = plang_dictionary_new(4);
    if (scope->_types == NULL) goto error;
    plang_dictionary_set_key_comparator(scope->_types,
                                        plang_token_identifier_comparator,
                                        NULL);

    scope->_variables = plang_dictionary_new(4);
    if (scope->_variables == NULL) goto error;
    plang_dictionary_set_key_comparator(scope->_variables,
                                        plang_token_identifier_comparator,
                                        NULL);

    scope->_procedures = plang_dictionary_new(4);
    if (scope->_procedures == NULL) goto error;
    plang_dictionary_set_key_comparator(scope->_procedures,
                                        plang_token_identifier_comparator,
                                        NULL);

    scope->_functions = plang_dictionary_new(4);
    if (scope->_functions == NULL) goto error;
    plang_dictionary_set_key_comparator(scope->_functions,
                                        plang_token_identifier_comparator,
                                        NULL);

    return scope;

error:
    plang_scope_free(scope);
    return NULL;
}


void
plang_scope_free(plang_scope_t PLANG_NULLABLE scope)
{
    if (scope == NULL) return;

    plang_dictionary_free(scope->_constants);
    plang_dictionary_free(scope->_types);
    plang_dictionary_free(scope->_variables);
    plang_dictionary_free(scope->_procedures);
    plang_dictionary_free(scope->_functions);
}


plang_scope_t PLANG_NULLABLE
plang_scope_get_parent(plang_scope_t scope)
{
    return scope->_parent;
}


bool
plang_scope_item_register(plang_dictionary_t dictionary,
                          plang_token_t identifier,
                          void *item)
{
    void *existing = plang_scope_item_lookup(dictionary, identifier);
    if (existing) return false;
    
    return plang_dictionary_set(dictionary, identifier, item);
}


void * PLANG_NULLABLE
plang_scope_item_lookup(plang_dictionary_t dictionary,
                        plang_token_t identifier)
{
    return plang_dictionary_get(dictionary, identifier);
}


typedef bool (*plang_item_comparator_t)(void *item,
                                        void * PLANG_NULLABLE context);


void * PLANG_NULLABLE
plang_scope_item_lookup_by_node(plang_dictionary_t dictionary,
                                plang_item_comparator_t comp,
                                void * PLANG_NULLABLE context)
{
    void *item = NULL;

    /* Do a linear search of the vlaues, for now. */
    plang_array_t values = plang_dictionary_copy_all_values(dictionary);
    if (values == NULL) return NULL;

    const size_t count = plang_array_get_count(values);
    for (size_t i = 0; i < count; i++) {
        void *one_item = plang_array_get_item(values, i);
        if (comp(one_item, context)) {
            item = one_item;
            break;
        }
    }

    plang_array_free(values);

    return item;
}


bool
plang_scope_constant_register(plang_scope_t scope,
                              plang_constant_t constant)
{
    plang_token_t identifier = plang_constant_get_identifier(constant);
    
    return plang_scope_item_register(scope->_constants,
                                     identifier,
                                     constant);
}


plang_constant_t PLANG_NULLABLE
plang_scope_constant_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents)
{
    plang_constant_t constant = NULL;
    
    constant = plang_scope_item_lookup(scope->_constants, identifier);
    
    if ((constant == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (constant == NULL);
             next = next->_parent)
        {
            constant = plang_scope_item_lookup(next->_constants,
                                               identifier);
        }
    }

    return constant;
}


plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_constants(plang_scope_t scope)
{
    return plang_dictionary_copy(scope->_constants);
}


bool
plang_scope_type_register(plang_scope_t scope,
                          plang_type_t type)
{
    plang_token_t identifier = plang_type_get_identifier(type);
    
    return plang_scope_item_register(scope->_types, identifier, type);
}


plang_type_t PLANG_NULLABLE
plang_scope_type_lookup(plang_scope_t scope,
                        plang_token_t identifier,
                        bool search_parents)
{
    plang_type_t type = NULL;
    
    type = plang_scope_item_lookup(scope->_types, identifier);

    if ((type == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (type == NULL);
             next = next->_parent)
        {
            type = plang_scope_item_lookup(next->_types, identifier);
        }
    }
    
    return type;
}


bool
plang_type_node_comparator(void *item,
                           void * PLANG_NULLABLE context)
{
    plang_type_t type = (plang_type_t) item;
    plang_node_t node = (plang_node_t) context;

    return (plang_type_get_node(type) == node);
}


plang_type_t
plang_scope_type_lookup_anonymous_type(plang_scope_t scope,
                                       plang_node_t node,
                                       bool search_parents)
{
    plang_type_t type = NULL;

    type = plang_scope_item_lookup_by_node(scope->_types,
                                           plang_type_node_comparator,
                                           node);

    if ((type == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (type == NULL);
             next = next->_parent)
        {
            type = plang_scope_item_lookup_by_node(next->_types,
                                                   plang_type_node_comparator,
                                                   node);
        }
    }

    return type;
}


plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_types(plang_scope_t scope)
{
    return plang_dictionary_copy(scope->_types);
}


bool
plang_scope_variable_register(plang_scope_t scope,
                              plang_variable_t variable)
{
    plang_token_t identifier = plang_variable_get_identifier(variable);

    return plang_scope_item_register(scope->_variables,
                                     identifier,
                                     variable);
}


plang_variable_t PLANG_NULLABLE
plang_scope_variable_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents)
{
    plang_variable_t variable = NULL;

    variable = plang_scope_item_lookup(scope->_variables,
                                       identifier);

    if ((variable == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (variable == NULL);
             next = next->_parent)
        {
            variable = plang_scope_item_lookup(next->_variables,
                                               identifier);
        }
    }

    return variable;
}


plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_variables(plang_scope_t scope)
{
    return plang_dictionary_copy(scope->_variables);
}


bool
plang_scope_procedure_register(plang_scope_t scope,
                               plang_procedure_t procedure)
{
    plang_token_t identifier = plang_procedure_get_identifier(procedure);

    return plang_scope_item_register(scope->_procedures,
                                     identifier,
                                     procedure);
}


plang_procedure_t PLANG_NULLABLE
plang_scope_procedure_lookup(plang_scope_t scope,
                             plang_token_t identifier,
                             bool search_parents)
{
    plang_procedure_t procedure = NULL;
    
    procedure = plang_scope_item_lookup(scope->_procedures,
                                        identifier);
    
    if ((procedure == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (procedure == NULL);
             next = next->_parent)
        {
            procedure = plang_scope_item_lookup(next->_procedures,
                                                identifier);
        }
    }
    
    return procedure;
}


plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_procedures(plang_scope_t scope)
{
    return plang_dictionary_copy(scope->_procedures);
}


bool
plang_scope_function_register(plang_scope_t scope,
                              plang_function_t function)
{
    plang_token_t identifier = plang_function_get_identifier(function);

    return plang_scope_item_register(scope->_functions,
                                     identifier,
                                     function);
}


plang_function_t PLANG_NULLABLE
plang_scope_function_lookup(plang_scope_t scope,
                            plang_token_t identifier,
                            bool search_parents)
{
    plang_function_t function = NULL;

    function = plang_scope_item_lookup(scope->_functions,
                                       identifier);

    if ((function == NULL) && search_parents) {
        for (plang_scope_t next = scope->_parent;
             (next != NULL) && (function == NULL);
             next = next->_parent)
        {
            function = plang_scope_item_lookup(next->_functions,
                                               identifier);
        }
    }

    return function;
}


plang_dictionary_t PLANG_NULLABLE
plang_scope_copy_functions(plang_scope_t scope)
{
    return plang_dictionary_copy(scope->_functions);
}


PLANG_SOURCE_END
