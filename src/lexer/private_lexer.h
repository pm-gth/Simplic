#ifndef PRIVATE_LEXER_H
#define PRIVATE_LEXER_H

#include "lexer.h"

// Text analizers
bool isAlpha(char c);
bool isNumber(char c);
bool isAlphaNumer(char c);

Token* createToken(TokenType type, const char* text);

#endif