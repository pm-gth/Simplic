#ifndef JUMPTABLE_H 
#define JUMPTABLE_H

#include "dataStructures/ast.h"
#include "simplicError.h"

typedef struct JumpTableEntry JumpTableEntry;

struct JumpTableEntry {
   char* identifier;
   SyntaxNode* address;
};

extern JumpTableEntry** jumpTable; // Table of identifiers and addresses

void initJumpTable(void);
void addJumpEntry(char* identifier, SyntaxNode* address, SimplicError* error);
SyntaxNode* getJumpAddress(char* identifier, SimplicError* error);
void deleteJumpTable(void);

typedef struct JumpStackNode JumpStackNode;

struct JumpStackNode {
   SyntaxNode* address;
   JumpStackNode* next;
};

extern JumpStackNode* jumpStackHead; // Keeps track of the place where the last jump was made

void initJumpStack(void);
void pushJump(SyntaxNode* address);
SyntaxNode* popJump(SimplicError* error);
bool isJumpStackEmpty(void);
void deleteJumpStack(void);

#endif