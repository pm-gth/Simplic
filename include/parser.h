#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "simplicError.h"
#include "simplic.h"

typedef enum {
    NODE_ASSIGN,
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
    char operator;  // For bin_ops
    int numberValue;    // For numbers
    char* string; // For strings
    SyntaxNode* left;
    SyntaxNode* right;
};

void freeSyntaxTree(SyntaxNode* tree);

// Parses a line of code from the token list and generates an AST used later for execution
SyntaxNode* parseTokenList(Token** tokenList, SimplicError* error);

#endif