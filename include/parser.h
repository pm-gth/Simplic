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
#include "simplic.h"

typedef enum {
    NODE_ASSIGN,
    NODE_UNASSIGN,
    NODE_PRINT,
    NODE_RETURN,
    NODE_NUMBER,
    NODE_STRING,
    NODE_VAR,
    NODE_INCREMENT,
    NODE_DECREMENT,
    NODE_BIN_OP
} NodeType;

// AST nodes, with different nodes to account for different tokens (vars, loops, instructions...)
typedef struct SyntaxNode SyntaxNode;
struct SyntaxNode {
    NodeType type;
    char varName[IDENTIFIER_SIZE]; // For variables
    char operator[BIN_OP_OPERATOR_SIZE];  // For bin_ops
    int numberValue;    // For numbers
    char* string; // For strings
    SyntaxNode* left;
    SyntaxNode* right;
};

void freeSyntaxTree(SyntaxNode* tree);

// Parses a line of code from the token list and generates an AST used later for execution
SyntaxNode* parseTokenList(Token** tokenList, SimplicError* error);

#endif