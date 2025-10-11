#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

#include "parser.h"

ParseResult makeResult(SyntaxNode* n);
static ParseResult makeError(SimplicError* err, const char* msg, int code);

Token* peek(); // Returns current token
Token advance(); // Pops out a token and return a copy of it

ParseResult parseFactor(SimplicError* error); // var or num
ParseResult parseTerm(SimplicError* error); // *, /
ParseResult parseExpr(SimplicError* error); // +, -

#endif