#ifndef LEXER_H
#define LEXER_H

/*
=======================================================================================
 The lexer parses the program text, given in the form of a string to a list of tokens.
 Tokens are way easier to work with than chars, one call to tokenizeSource() stores
 all the program's tokens inside a linked list.
 This list will then be processed by the parser
=======================================================================================
*/
#include "simplic.h"
#include "simplicError.h"

typedef enum {
    TOKEN_SET,
    TOKEN_PRINT,
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
    TOKEN_EQ, // EQ comparation
    TOKEN_NEQ,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_GEQ,
    TOKEN_LEQ,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_NEWLINE,
    TOKEN_EOF
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char name[IDENTIFIER_SIZE];
	struct Token* next;
    char* string; // In case of string literal
};

// Token linked list functions
Token* initTokenList(void);
int emptyTokenList(Token** tokenList); // -1 if list is already empty

int addTokenToTail(Token** tokenList, TokenType type, const char* name, char* string); // -1 if fail
int removeTokenFromHead(Token** tokenList); // -1 if list is already empty
void printTokens(Token* tokenList);

// Receives null-terminated string as input, creates linked list of tokens from a source code
void tokenizeSource(Token** tokenList, const char* src, SimplicError* error);

#endif