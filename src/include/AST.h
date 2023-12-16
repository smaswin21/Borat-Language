#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum {
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_COMPOUND,
        AST_NOOP,
        AST_IF,          // Added AST node type for 'if' statements
        AST_ELSE,        // Added AST node type for 'else' statements
        AST_WHILE        // Added AST node type for 'while' statements
    } type;

    struct SCOPE_STRUCT* scope;

    /* AST_VARIABLE_DEFINITION */
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;

    /* AST_FUNCTION_DEFINITION */
    struct AST_STRUCT* function_definition_body;
    char* function_definition_name;
    struct AST_STRUCT** function_definition_args;
    size_t function_definition_args_size;

    /* AST_VARIABLE */
    char* variable_name;

    /* AST_FUNCTION_CALL */
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    /* AST_STRING */
    char* string_value;

    /* AST_COMPOUND */
    struct AST_STRUCT** compound_value;
    size_t compound_size;

    /* AST_IF */
    struct AST_STRUCT* if_condition;      // Condition for 'if' statement
    struct AST_STRUCT* if_body;           // Body of 'if' statement

    /* AST_ELSE */
    struct AST_STRUCT* else_body;         // Body of 'else' statement

    /* AST_WHILE */
    struct AST_STRUCT* while_condition;   // Condition for 'while' statement
    struct AST_STRUCT* while_body;        // Body of 'while' statement
} AST_T;

AST_T* init_ast(int type);

#endif
