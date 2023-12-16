#ifndef TOKEN_H
#define TOKEN_H

typedef struct TOKEN_STRUCT
{
    enum
    {
        TOKEN_ID,
        TOKEN_EQUALS,
        TOKEN_STRING,
        TOKEN_SEMI,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_RBRACE,
        TOKEN_LBRACE,
        TOKEN_COMMA,
        TOKEN_EOF,
        TOKEN_VAR,      // for "Suck!"
        TOKEN_IF,       // for "Hey, what your name?"
        TOKEN_ELSE,     // for "Who you with?"
        TOKEN_WHILE     // for "Uh, you mean to the restroom?"
    } type;

    char* value;
} token_T;

token_T* init_token(int type, char* value);

#endif
