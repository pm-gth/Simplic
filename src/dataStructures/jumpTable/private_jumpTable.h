#ifndef PRIVATE_JUMPTABLE_H
#define PRIVATE_JUMPTABLE_H

#include "dataStructures/ast.h"
#include "dataStructures/jumpTable.h"

static void jumpTableEnsureCapacity(void);
static JumpTableEntry* newJumpTableEntry(char* identifier, SyntaxNode* address);
static void freeJumpTableEntry(JumpTableEntry* entry);

static JumpStackNode* newJumpStackNode(SyntaxNode* address);
static void freeJumpStackNode(JumpStackNode* node);

#endif