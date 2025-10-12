#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

#include "parser.h"

// Structure used to wrap result trees and check for errors
typedef struct ParseResult ParseResult;
struct ParseResult{
    SyntaxNode* node;
    bool hasError;
};

// Wrapper functions, used to return nodes or errors
static ParseResult makeResult(SyntaxNode* n);
static ParseResult makeError(SimplicError* err, const char* msg, int code);

static Token* peek(Token** tokenList); // Returns current token
static Token advance(Token** tokenList); // Pops out a token and returns a copy of it

static bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b); // Used to compare AST in tests

// Node generators, used to determine the kind of node to create based on the token list
static ParseResult parseStatement(Token** tokenList, SimplicError* error); // generates instruction nodes
static ParseResult parseFactor(Token** tokenList, SimplicError* error); // generates variable, number or string nodes
static ParseResult parseTerm(Token** tokenList, SimplicError* error); // takes care of * and / nodes
static ParseResult parseExpr(Token** tokenList, SimplicError* error); // takes care of + and - nodes
static ParseResult parseRelational(Token** tokenList, SimplicError* error); // Takes care of relational comparisons (<, >, <=, >=)
static ParseResult parseEquality(Token** tokenList, SimplicError* error); // Takes care of equality ops == and !=
static ParseResult parseLogical(Token** tokenList, SimplicError* error); // Takes care of logical ops && and ||
static ParseResult parseLowestPrecedenceOperation(Token** tokenList, SimplicError* error); // Wrapper to call lowest priority parsing

static SyntaxNode* initNode(void); // Used to create a node, sets to NULL its fields

#endif