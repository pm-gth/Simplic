#ifndef INTERPRETER_H
#define INTERPRETER_H

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