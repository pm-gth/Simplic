#ifndef LEXER_H
#define LEXER_H

#include "simplic.h"

typedef enum {
    TOKEN_SET,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_EQUALS,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_NEWLINE,
    TOKEN_EOF
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char text[IDENTIFIER_SIZE];
	struct Token* next;
};

// Text analizers
bool isAlpha(char c);
bool isNumber(char c);
bool isAlphaNumer(char c);

// Token linked list functions
void initTokenList(Token** tokenList);
Token* createToken(TokenType type, const char* text);
int addTokenToTail(Token** tokenList, TokenType type, const char* text); // -1 if fail
int removeTokenFromHead(Token** tokenList); // -1 if list is already empty
void printTokens(Token* tokenList);
int removeAllTokens(Token** tokenList); // -1 if list is already empty

// Receives null-terminated string as input, creates linked list of tokens from a source code
void tokenizeSource(Token** tokenList, const char* src);

#endif