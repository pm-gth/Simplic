#ifndef PRIVATE_LEXER_H
#define PRIVATE_LEXER_H

#include "lexer.h"

// Text analizers
static bool isAlpha(char c);
static bool isNumber(char c);
static bool isAlphaNumer(char c);

static Token* createToken(TokenType type, const char* text);

#endif