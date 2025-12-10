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
#include "dataStructures/token.h"

// Receives null-terminated string as input, creates linked list of tokens from a source code
void tokenizeSource(Token** tokenList, const char* src, SimplicError* error);

#endif