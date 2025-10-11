#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "simplic.h"
#include "parser.h"
#include "simplicError.h"

typedef struct MemoryCell MemoryCell;
struct MemoryCell {
    char name[IDENTIFIER_SIZE];
    int value;
    char* strPtr; // In case of string
    MemoryCell* next; // In case of name collision
};

extern MemoryCell* MemoryBank[HASH_TABLE_SIZE]; // Hashmap of variables

typedef enum {
    VALUE_INT,
    VALUE_STR,
    VALUE_VOID
} ValueType;

// Wrapper type for eval(), contains the result of the last evaluation
typedef struct Value Value;
struct Value {
    ValueType type;
    int integer;
    char* string;
    bool receivedReturn;
};

void initMemoryBank();
void emptyMemoryBank(); // Empties the variable bank, call at exit

// Receives an AST as input and computes it recursively, returning the value of each node
Value eval(SyntaxNode* node, SimplicError* error);

#endif