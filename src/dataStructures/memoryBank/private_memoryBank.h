#ifndef PRIVATE_MEMORYBANK_H
#define PRIVATE_MEMORYBANK_H

#include "dataStructures/memoryBank.h"

// Wrapper functions for the bank, return values or errors
static BankResult makeResultInt(int n);
static BankResult makeResultStr(char* s);
static BankResult makeError(SimplicError* err, SimplicErrorType code, const char* fmt, ...);

static unsigned long stringHash(const char *str); // Hash function for strings

static MemoryCell* initMemCell(void); // Creates a memory cell with its default values initialized

#endif