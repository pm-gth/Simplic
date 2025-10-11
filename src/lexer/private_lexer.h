#ifndef PRIVATE_LEXER_H
#define PRIVATE_LEXER_H

#include "lexer.h"

// Used to tokenize the code
static bool isAlpha(char c);
static bool isNumber(char c);
static bool isAlphaNumer(char c);

// Creates a token, receives its type, name (in case it's a variable)
// and a string in case in was a string literal
static Token* createToken(TokenType type, const char* name, char* string);

#endif