#ifndef PARSER_H
#define PARSER_H

/*
=======================================================================================
 The parsers reads tokens from the list made by the lexer. Its purpose is to transform
 that list into different ASTs (Abstract Syntax Trees). When parseTokenList() is
 called, it generates an AST for the current line of code, popping out those tokens
 from the token list in the process.
 Each node made by the parser has different properties depending on its function,
 some represent instructions like SET or PRINT, other are string literals or variables.
 The parser consists on functions which call themselves recursively, each one tries
 to resolve an expression of certain "predence" be it an instruction, a factor
 (atomic type) or a complex expression like *, /, + or -
=======================================================================================
*/

#include "lexer.h"
#include "simplicError.h"
#include "dataStructures/ast.h"
#include "dataStructures/jumpTable.h"

// Parses a line of code from the token list and generates an AST used later for execution
SyntaxNode* parseLineOfCode(Token** tokenList, SimplicError* error);
void parseFullCode(Token** tokenList, SimplicError* error);

#endif