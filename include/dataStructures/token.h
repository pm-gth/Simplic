#ifndef TOKEN_H
#define TOKEN_H

/*
=======================================================================================
 Token type used to represent code fragments, created and stored in a queue by the
 lexer, then sent as input to the parser
=======================================================================================
*/

#include "simplic.h"

typedef enum {
    TOKEN_ERROR_TOKEN, // For error handling
    TOKEN_SET,
    TOKEN_UNSET,
    TOKEN_PRINT,
    TOKEN_PRINTLN,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_EQUALS, // Symbol '='
    TOKEN_EQ, // EQ comparator
    TOKEN_NEQ,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_GEQ,
    TOKEN_LEQ,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_EOF
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char name[IDENTIFIER_SIZE];
	struct Token* next;
    char* string; // In case of string literal
};

// Creates a token, receives its type, name (in case it's a variable)
// and a string in case in was a string literal
Token* createToken(TokenType type, const char* name, char* string);

// Token queue functions
Token* initTokenQueue(void);
int deleteTokenQueue(Token** tokenList); // -1 if list is already empty

int enqueueToken(Token** tokenList, TokenType type, const char* name, char* string); // -1 if fail
Token dequeueToken(Token** tokenList); // Pops out a token and returns a copy of it
Token* peekTokenQueue(Token** tokenList); // Returns reference to current token

void printTokenQueue(Token* tokenList);

#endif