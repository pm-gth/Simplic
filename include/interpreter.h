#ifndef INTERPRETER_H
#define INTERPRETER_H

/*
=======================================================================================
 The interpreter is the part of the... interpreter that executes the code. It receives
 ASTs from the parser and executes them recursively, deciding what to do at each node.
 Some nodes are instructions which have their operands as child nodes, this child
 nodes can also be instructions themselves and so need to be solved first.
 The variables are stored at runtime in the VariableBank, this is a hashTable that
 uses the variable's names as keys. A variable can store different types of data
 (Integers or Strings for now). After the evaluation the AST used is deleted.
=======================================================================================
*/

#include "simplic.h"
#include "parser.h"
#include "simplicError.h"

typedef enum {
    VALUE_INT,
    VALUE_STR,
    VALUE_VOID
} ValueType;

// Wrapper type for eval(), contains the result of the last evaluation
typedef struct SimplicValue SimplicValue;
struct SimplicValue {
    ValueType type;
    int integer;
    char* string;
    bool receivedReturn;
};

void initMemoryBank(void);
void emptyMemoryBank(void); // Empties the variable bank, call at exit

// Receives an AST as input and computes it recursively, returning the value of each node
SimplicValue eval(SyntaxNode* node, SimplicError* error);

#endif