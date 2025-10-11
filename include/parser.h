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
    NODE_VAR,
    NODE_INCREMENT,
    NODE_DECREMENT,
    NODE_BIN_OP
} NodeType;

extern Token* tokenList; // Global token List

typedef struct SyntaxNode SyntaxNode;
struct SyntaxNode {
    NodeType type;
    char varName[IDENTIFIER_SIZE];   // For variables
    char operator;  // For bin_ops
    int numberValue;    // For numbers
    SyntaxNode* left;
    SyntaxNode* right;
};

bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b);
void freeSyntaxTree(SyntaxNode* tree);

// Structure used to wrap result trees and check for errors
typedef struct ParseResult ParseResult;
struct ParseResult{
    SyntaxNode* node;
    bool hasError;
};

ParseResult parseStatement(SimplicError* error); // instruction

int eval(SyntaxNode* node, SimplicError* error);

#endif