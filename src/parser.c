#include "include/parser.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>
#include "include/AST.h"

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->prev_token = parser->current_token;

    parser->scope = init_scope();

    return parser;
}

void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        printf(
            "Unexpected token `%s`, with type %d",
            parser->current_token->value,
            parser->current_token->type
        );
        exit(1);
    }
}

AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
    return parser_parse_statements(parser, scope);
}

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_ID: return parser_parse_id(parser, scope);
    }

    return init_ast(AST_NOOP);
}

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement)
        {
            compound->compound_size += 1;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT*)
            );
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
    }

    return compound;
}

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_STRING: return parser_parse_string(parser, scope);
        case TOKEN_ID: return parser_parse_id(parser, scope);
    }

    return init_ast(AST_NOOP);
}

AST_T* parser_parse_factor(parser_T* parser, scope_T* scope)
{
}

AST_T* parser_parse_term(parser_T* parser, scope_T* scope)
{
}

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
    AST_T* function_call = init_ast(AST_FUNCTION_CALL);

    function_call->function_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LPAREN); 

    function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_expr = parser_parse_expr(parser, scope);
    function_call->function_call_arguments[0] = ast_expr;
    function_call->function_call_arguments_size += 1;

    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        AST_T* ast_expr = parser_parse_expr(parser, scope);
        function_call->function_call_arguments_size += 1;
        function_call->function_call_arguments = realloc(
            function_call->function_call_arguments,
            function_call->function_call_arguments_size * sizeof(struct AST_STRUCT*)
        );
        function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expr;
    }
    parser_eat(parser, TOKEN_RPAREN);

    function_call->scope = scope;

    return function_call;
}

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_ID); // function

    char* function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
            strlen(function_name) + 1, sizeof(char)
    );
    strcpy(ast->function_definition_name, function_name);

    parser_eat(parser, TOKEN_ID); // function name

    parser_eat(parser, TOKEN_LPAREN);

    ast->function_definition_args =
        calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* arg = parser_parse_variable(parser, scope);
    ast->function_definition_args_size += 1;
    ast->function_definition_args[ast->function_definition_args_size-1] = arg;

    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        ast->function_definition_args_size += 1;

        ast->function_definition_args =
            realloc(
                    ast->function_definition_args,
                    ast->function_definition_args_size * sizeof(struct AST_STRUCT*)
                   );

        AST_T* arg = parser_parse_variable(parser, scope);
        ast->function_definition_args[ast->function_definition_args_size-1] = arg;
    }

    parser_eat(parser, TOKEN_RPAREN);
    
    parser_eat(parser, TOKEN_LBRACE);
    
    ast->function_definition_body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RBRACE);

    ast->scope = scope;

    return ast;
}

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
    char* token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name or function call name

    if (parser->current_token->type == TOKEN_LPAREN)
        return parser_parse_function_call(parser, scope);

    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
    AST_T* ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}

AST_T* parser_parse_id(parser_T* parser, scope_T* scope) {
    if (strcmp(parser->current_token->value, "Suck") == 0) {
        return parser_parse_variable_definition(parser, scope);
    } else if (strcmp(parser->current_token->value, "Heywhatyourname") == 0) {
        return parser_parse_if_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "Whoyouwith") == 0) {
        return parser_parse_else_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "Uhrestroom?") == 0) {
        return parser_parse_while_statement(parser, scope);
    } else if (strcmp(parser->current_token->value, "function") == 0) {
        return parser_parse_function_definition(parser, scope);
    } else {
        return parser_parse_variable(parser, scope);
    }
}


AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope) {
    // Consume the "Suck!" token
    parser_eat(parser, TOKEN_VAR); // Assuming TOKEN_VAR represents "Suck!"

    // Expect a variable name after "Suck!"
    char* variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    // Expect an equals sign
    parser_eat(parser, TOKEN_EQUALS);

    // Parse the expression on the right-hand side of the equals sign
    AST_T* value = parser_parse_expr(parser, scope);

    // Construct the variable definition AST node
    AST_T* var_def = init_ast(AST_VARIABLE_DEFINITION);
    var_def->variable_definition_variable_name = variable_name;
    var_def->variable_definition_value = value;

    return var_def;
}

AST_T* parser_parse_if_statement(parser_T* parser, scope_T* scope) {
    // Consume the "Hey, what your name?" token
    parser_eat(parser, TOKEN_IF);

    // Parse the condition inside parentheses
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* condition = parser_parse_expr(parser, scope);
    parser_eat(parser, TOKEN_RPAREN);

    // Parse the statement block inside braces
    parser_eat(parser, TOKEN_LBRACE);
    AST_T* body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);

     // Add the missing import statement for the AST_T structure.
    AST_T* if_statement = init_ast(AST_IF);
    // Correcting the member name
    if_statement->if_condition = condition;
    if_statement->if_body = body;

    return if_statement;
}

AST_T* parser_parse_else_statement(parser_T* parser, scope_T* scope) {
    // Consume the "Who you with?" token
    parser_eat(parser, TOKEN_ELSE);

    // Parse the statement block inside braces
    parser_eat(parser, TOKEN_LBRACE);
    AST_T* body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);

    AST_T* else_statement = init_ast(AST_ELSE);
    else_statement->else_body = body;

    return else_statement;
}

AST_T* parser_parse_while_statement(parser_T* parser, scope_T* scope) {
    // Consume the "Uh, you mean to the restroom?" token
    parser_eat(parser, TOKEN_WHILE);

    // Parse the condition inside parentheses
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* condition = parser_parse_expr(parser, scope);
    parser_eat(parser, TOKEN_RPAREN);

    // Parse the statement block inside braces
    parser_eat(parser, TOKEN_LBRACE);
    AST_T* body = parser_parse_statements(parser, scope);
    parser_eat(parser, TOKEN_RBRACE);

    #include "include/AST.h" // Add the missing import statement for the AST_T structure.
    AST_T* while_statement = init_ast(AST_WHILE);
    while_statement->while_condition = condition;
    while_statement->while_body = body;

    return while_statement;
}
