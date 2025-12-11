#ifndef PARSER_PRIVATE_H
#define PARSER_PRIVATE_H

#include "parser.h"
#include "dataStructures/token.h"
#include "simplicError.h"

// Structure used to wrap result trees and check for errors
typedef struct ParseResult ParseResult;
struct ParseResult{
    SyntaxNode* node;
    bool hasError;
};

// Wrapper functions, used to return nodes or errors
static ParseResult makeResult(SyntaxNode* n);
ParseResult makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...);

TokenType findIfBlockDelimiter(Token** tokenList); // Determines if the IF block delimiter is ELSE or FI (or none)

// Node generators, used to determine the kind of node to create based on the token list
static ParseResult parseStatement(Token** tokenList, SimplicError* error); // generates instruction nodes
static ParseResult parseFactor(Token** tokenList, SimplicError* error); // generates variable, number or string nodes
static ParseResult parseTerm(Token** tokenList, SimplicError* error); // takes care of * and / nodes
static ParseResult parseExpr(Token** tokenList, SimplicError* error); // takes care of + and - nodes
static ParseResult parseRelational(Token** tokenList, SimplicError* error); // Takes care of relational comparisons (<, >, <=, >=)
static ParseResult parseEquality(Token** tokenList, SimplicError* error); // Takes care of equality ops == and !=
static ParseResult parseLogical(Token** tokenList, SimplicError* error); // Takes care of logical ops && and ||
static ParseResult parseLowestPrecedenceOperation(Token** tokenList, SimplicError* error); // Wrapper to call lowest priority parsing
SyntaxNode* parseBlock(Token** tokenList, SimplicError* error, TokenType endToken); // takes care of code blocks

#endif