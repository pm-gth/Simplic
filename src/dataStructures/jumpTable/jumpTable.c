#include "private_jumpTable.h"

JumpTableEntry** jumpTable;
int jumpTableIndex;
int jumpTableSize;

void initJumpTable(void) {
   jumpTable = malloc(sizeof(JumpTableEntry*) * STARTING_JUMP_TABLE_CAPACITY);

   jumpTableIndex = 0;
   jumpTableSize = STARTING_JUMP_TABLE_CAPACITY;
}

void jumpTableEnsureCapacity() {
    if(jumpTableIndex >= jumpTableSize) {
        astArray = realloc(jumpTable, jumpTableSize + 5);
        jumpTableSize += 10;

        for(size_t i = jumpTableIndex; i < jumpTableSize; i++) {
            jumpTable[i] = NULL;
        }
    }
}

JumpTableEntry* newJumpTableEntry(char* identifier, SyntaxNode* address) {
   JumpTableEntry* entry = malloc(sizeof(JumpTableEntry));
   entry->identifier = strdup(identifier);
   entry->address = address;

   return entry;
}

void addJumpEntry(char* identifier, SyntaxNode* address, SimplicError* error) {
   jumpTableEnsureCapacity();

   for(int i = 0; i < jumpTableIndex; i++) {
      if(strcmp(identifier, jumpTable[i]->identifier) == 0) {
         setError(error, ERROR_SYMBOL_REASIGNATION , "Tried to reasign an address to jump identifier %s", identifier);
         return;
      }
   }

   jumpTable[jumpTableIndex++] = newJumpTableEntry(identifier, address);
}

SyntaxNode* getJumpAddress(char* identifier, SimplicError* error) {
   for(int i = 0; i < jumpTableIndex; i++) {
      if(strcmp(identifier, jumpTable[i]->identifier) == 0) {
         return jumpTable[i]->address;
      }
   }

   setError(error, ERROR_ACCESS_TO_UNDECLARED_VAR, "Undefined jump identifier %s", identifier);
   return NULL;
}

static void freeJumpTableEntry(JumpTableEntry* entry) {
   free(entry->identifier);
   free(entry);
}

void deleteJumpTable(void) {
   for(int i = 0; i < jumpTableIndex; i++) {
      freeJumpTableEntry(jumpTable[i]);
   }
   free(jumpTable);
}

JumpStackNode* jumpStackHead;

void initJumpStack(void) {
   jumpStackHead = NULL;
}

JumpStackNode* newJumpStackNode(SyntaxNode* address) {
   JumpStackNode* node = malloc(sizeof(JumpStackNode));
   node->address = address;
   node->next = NULL;

   return node;
}

void pushJump(SyntaxNode* address) {
   JumpStackNode* node = newJumpStackNode(address);
   node->next = jumpStackHead;
   jumpStackHead = node;
}

static void freeJumpStackNode(JumpStackNode* node) {
   free(node);
}

SyntaxNode* popJump(SimplicError* error) {
    if (jumpStackHead == NULL)
        setError(error, ERROR_EMPTY_JUMP_STACK, "Tried to pop empty JumpStack, undefined return address");

    JumpStackNode* node = jumpStackHead;
    SyntaxNode* addr = node->address;

    jumpStackHead = node->next;

    freeJumpStackNode(node);

    return addr;
}

bool isJumpStackEmpty(void) {
   return jumpStackHead == NULL;
}

void deleteJumpStack(void) {
   SimplicError* error = initError();

   while(!isJumpStackEmpty()) {
      popJump(error);
   }

   free(error);
}