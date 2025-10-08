#ifndef LEXER_H
#define LEXER_H

#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define MAX_TOKEN_TEXT 64

typedef enum TokenType {
    TOKEN_SET,
    TOKEN_PRINT,
    TOKEN_VAR,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_EQUALS,
    TOKEN_NEWLINE,
    TOKEN_EOF
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char text[MAX_TOKEN_TEXT];
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

// Receivesnull-terminated string as input, creates linked list of tokens from a source code
void tokenizeSource(Token** tokenList, const char* src);

#endif