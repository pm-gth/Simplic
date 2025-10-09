#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "simplic.h"
#include "parser.h"

typedef struct MemoryCell MemoryCell;
struct MemoryCell {
        char name[IDENTIFIER_SIZE];
        int value;
};

#endif