#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

#include "parser.h"

static ParseResult makeResult(SyntaxNode* n);
static ParseResult makeError(SimplicError* err, const char* msg, int code);

static Token* peek(); // Returns current token
static Token advance(); // Pops out a token and return a copy of it

static ParseResult parseFactor(SimplicError* error); // var or num
static ParseResult parseTerm(SimplicError* error); // *, /
static ParseResult parseExpr(SimplicError* error); // +, -

#endif