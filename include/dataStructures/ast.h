#ifndef AST_H 
#define AST_H

/*
=======================================================================================
Abstract Syntax Tree used to represent lines of the program, ASTs are made by the
parser then sent to the interpreter.
Each node represents one kind operation to be done by the interpreter, this operations
have different subnodes that store the operands, this operands can be other operations
themselves. More information about how each type of node stores its operands can be 
found in parser.c
=======================================================================================
*/

#include "simplic.h"

typedef enum {
    NODE_ASSIGN,
    NODE_UNASSIGN,
    NODE_PRINT,
    NODE_PRINTLN,
    NODE_RETURN,
    NODE_NUMBER,
    NODE_STRING,
    NODE_VAR,
    NODE_INCREMENT,
    NODE_DECREMENT,
    NODE_BIN_OP,
    NODE_BLOCK,
    NODE_WHILE,
    NODE_IF
} NodeType;

// AST nodes, with different nodes to account for different tokens (vars, loops, instructions...)
typedef struct SyntaxNode SyntaxNode;
struct SyntaxNode {
    NodeType type;
    char varName[IDENTIFIER_SIZE]; // For variables
    char operator[BIN_OP_OPERATOR_SIZE];  // For bin_ops
    int numberValue;    // For numbers
    char* string; // For strings
    SyntaxNode** blockStatements; // List of Syntax trees, used to store blocks of instructions
    SyntaxNode* subnodeA; // Subnodes used to store different kind of data depending on the father's node type
    SyntaxNode* subnodeB;
    SyntaxNode* subnodeC;
};

SyntaxNode* initNode(void); // Used to create a node, sets to NULL its fields

void freeSyntaxTree(SyntaxNode* tree);
bool compareSyntaxTree(SyntaxNode* a, SyntaxNode* b); // Used to compare AST in tests

extern SyntaxNode** astArray; // List of AST trees, each one containing a line from the script

void initAstArray(void);
void addLineAstArray(SyntaxNode* tree); // Add AST tree to the AST-Array
void deleteAstArray(void); // Deallocate AST-Array
size_t astArraySize(void);

#endif